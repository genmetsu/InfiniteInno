/*
Physics Effects Copyright(C) 2012 Sony Computer Entertainment Inc.
All rights reserved.

Physics Effects is open software; you can redistribute it and/or
modify it under the terms of the BSD License.

Physics Effects is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the BSD License for more details.

A copy of the BSD License is distributed with
Physics Effects under the filename: physics_effects_license.txt
*/
#include "stdafx.h"

#include "../../../include/physics_effects/base_level/base/pfx_vec_utils.h"
#include "pfx_contact_tri_mesh_capsule.h"
#include "pfx_intersect_common.h"
#include "pfx_mesh_common.h"
#include "pfx_decoded_tri_mesh.h"

namespace sce {
namespace PhysicsEffects {

// 分離軸が見つかった場合はすぐに処理を抜けるため最短距離が返されるわけではないことに注意

/*
	○ カプセル分離軸(x19)
	面法線
	カプセル軸 x エッジ0
	カプセル軸 x エッジ1
	カプセル軸 x エッジ2
	((カプセル点0-面点0) x エッジ0) x エッジ0
	((カプセル点0-面点1) x エッジ1) x エッジ1
	((カプセル点0-面点2) x エッジ2) x エッジ2
	((カプセル点1-面点0) x エッジ0) x エッジ0
	((カプセル点1-面点1) x エッジ1) x エッジ1
	((カプセル点1-面点2) x エッジ2) x エッジ2
	((面点0-カプセル点0) x カプセル軸) x カプセル軸
	((面点1-カプセル点0) x カプセル軸) x カプセル軸
	((面点2-カプセル点0) x カプセル軸) x カプセル軸
	面点0-カプセル点0
	面点1-カプセル点0
	面点2-カプセル点0
	面点0-カプセル点1
	面点1-カプセル点1
	面点2-カプセル点1
 */

#define CHECK_SAT(axis,AMin,AMax,BMin,BMax) \
{\
	PfxFloat d1 = AMin - BMax;\
	PfxFloat d2 = BMin - AMax;\
	if(distMin < d1) {\
		distMin = d1;\
		axisMin = sepAxis;\
	}\
	if(distMin < d2) {\
		distMin = d2;\
		axisMin = -sepAxis;\
	}\
	if(d1 > 0.0f || d2 > 0.0f) {\
		return true;\
	}\
}

static
bool pfxContactTriangleCapsule(PfxContactCache &contacts,PfxUInt32 facetId,
							const PfxVector3 &normal,const PfxVector3 &p0,const PfxVector3 &p1,const PfxVector3 &p2,
							const PfxFloat thickness,const PfxFloat angle0,const PfxFloat angle1,const PfxFloat angle2,
							PfxUInt32 edgeChk,
							PfxFloat capsuleRadius,const PfxVector3 &capP0,const PfxVector3 &capP1)
{
	const PfxFloat epsilon = 0.00001f;

	// 最も浅い貫通深度とそのときの分離軸
	PfxFloat distMin = -SCE_PFX_FLT_MAX;
	PfxVector3 axisMin(0.0f);

	//-------------------------------------------
	// １．分離軸判定
	{
		PfxVector3 facetPnts[6] = {
			p0,p1,p2,p0-thickness*normal,p1-thickness*normal,p2-thickness*normal
		};

		PfxVector3 sideNml[3] = {
			normalize(cross((facetPnts[1] - facetPnts[0]),normal)),
			normalize(cross((facetPnts[2] - facetPnts[1]),normal)),
			normalize(cross((facetPnts[0] - facetPnts[2]),normal)),
		};


		const PfxVector3 capPnts[2] = {
			capP0,capP1
		};

		{
			const PfxVector3 &sepAxis = normal;

			// 分離平面
			PfxPlane plane(sepAxis,facetPnts[0]);

			// Capsule(B)を分離軸に投影して範囲を取得
			PfxFloat test1,test2,BMin,BMax;
			test1 = plane.onPlane(capP0);
			test2 = plane.onPlane(capP1);
			BMax = SCE_PFX_MAX(test1,test2) + capsuleRadius;
			BMin = SCE_PFX_MIN(test1,test2) - capsuleRadius;

			// 判定
			if(BMin > 0.0f || BMax < -thickness) {
				return false;
			}

			if(distMin < BMin) {
				distMin = BMin;
				axisMin = -sepAxis;
			}
		}

		//-------------------------------------------
		// カプセル軸 x 面エッジ0,1,2
		// ※Triangles座標系 (Aローカル)

		{
			for(int e=0;e<3;e++) {
				PfxVector3 sepAxis = cross(capP1-capP0,facetPnts[(e+1)%3]-facetPnts[e]);
				PfxFloat l=length(sepAxis);
				if(l < 0.00001f) continue;
				sepAxis /= l;
				
				//if(dot(normal,sepAxis) > 0.0f)
				//	sepAxis = -sepAxis;
				
				// Triangleを分離軸に投影
				PfxFloat AMin=SCE_PFX_FLT_MAX,AMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts6(facetPnts,sepAxis,AMin,AMax);
				
				// カプセルを分離軸に投影
				PfxFloat BMin=SCE_PFX_FLT_MAX,BMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts2(capPnts,sepAxis,BMin,BMax);
				BMin -= capsuleRadius;
				BMax += capsuleRadius;
				
				CHECK_SAT(sepAxis,AMin,AMax,BMin,BMax)
			}
		}

		//-------------------------------------------
		// ((カプセル点0-面点0,1,2) x エッジ0,1,2) x エッジ0,1,2
		// ※Triangles座標系 (Aローカル)
		
		{
			for(int e=0;e<3;e++) {
				PfxVector3 edge = facetPnts[(e+1)%3]-facetPnts[e];
				PfxVector3 sepAxis = normalize(cross(cross(capP0-facetPnts[e],edge),edge));
				
				//if(dot(normal,sepAxis) > 0.0f)
				//	sepAxis = -sepAxis;
				
				// Triangleを分離軸に投影
				PfxFloat AMin=SCE_PFX_FLT_MAX,AMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts6(facetPnts,sepAxis,AMin,AMax);
				
				// カプセルを分離軸に投影
				PfxFloat BMin=SCE_PFX_FLT_MAX,BMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts2(capPnts,sepAxis,BMin,BMax);
				BMin -= capsuleRadius;
				BMax += capsuleRadius;
				
				CHECK_SAT(sepAxis,AMin,AMax,BMin,BMax)
			}
		}

		//-------------------------------------------
		// ((カプセル点1-面点0,1,2) x エッジ0,1,2) x エッジ0,1,2
		// ※Triangles座標系 (Aローカル)
		
		{
			for(int e=0;e<3;e++) {
				PfxVector3 edge = facetPnts[(e+1)%3]-facetPnts[e];
				PfxVector3 sepAxis = normalize(cross(cross(capP1-facetPnts[e],edge),edge));
				
				//if(dot(normal,sepAxis) > 0.0f)
				//	sepAxis = -sepAxis;
				
				// Triangleを分離軸に投影
				PfxFloat AMin=SCE_PFX_FLT_MAX,AMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts6(facetPnts,sepAxis,AMin,AMax);
				
				// カプセルを分離軸に投影
				PfxFloat BMin=SCE_PFX_FLT_MAX,BMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts2(capPnts,sepAxis,BMin,BMax);
				BMin -= capsuleRadius;
				BMax += capsuleRadius;
				
				CHECK_SAT(sepAxis,AMin,AMax,BMin,BMax)
			}
		}

		//-------------------------------------------
		// ((面点0,1,2-カプセル点0) x カプセル軸) x カプセル軸
		// ※Triangles座標系 (Aローカル)

		{
			for(int e=0;e<3;e++) {
				PfxVector3 capdir = capP1-capP0;
				PfxVector3 sepAxis = cross(cross(facetPnts[e]-capP0,capdir),capdir);
				PfxFloat l=length(sepAxis);
				if(l < 0.00001f) continue;
				sepAxis /= l;

				//if(dot(normal,sepAxis) > 0.0f)
				//	sepAxis = -sepAxis;

				// Triangleを分離軸に投影
				PfxFloat AMin=SCE_PFX_FLT_MAX,AMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts6(facetPnts,sepAxis,AMin,AMax);
				
				// カプセルを分離軸に投影
				PfxFloat BMin=SCE_PFX_FLT_MAX,BMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts2(capPnts,sepAxis,BMin,BMax);
				BMin -= capsuleRadius;
				BMax += capsuleRadius;
				
				CHECK_SAT(sepAxis,AMin,AMax,BMin,BMax);
			}
		}

		//-------------------------------------------
		// 面点0,1,2-カプセル点0
		// ※Triangles座標系 (Aローカル)

		{
			for(int e=0;e<3;e++) {
				PfxVector3 sepAxis = normalize(facetPnts[e]-capP0);

				// Triangleを分離軸に投影
				PfxFloat AMin=SCE_PFX_FLT_MAX,AMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts6(facetPnts,sepAxis,AMin,AMax);
				
				// カプセルを分離軸に投影
				PfxFloat BMin=SCE_PFX_FLT_MAX,BMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts2(capPnts,sepAxis,BMin,BMax);
				BMin -= capsuleRadius;
				BMax += capsuleRadius;
				
				CHECK_SAT(sepAxis,AMin,AMax,BMin,BMax);
			}
		}

		//-------------------------------------------
		// 面点0,1,2-カプセル点1
		// ※Triangles座標系 (Aローカル)

		{
			for(int e=0;e<3;e++) {
				PfxVector3 sepAxis = normalize(facetPnts[e]-capP1);
				
				// Triangleを分離軸に投影
				PfxFloat AMin=SCE_PFX_FLT_MAX,AMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts6(facetPnts,sepAxis,AMin,AMax);
				
				// カプセルを分離軸に投影
				PfxFloat BMin=SCE_PFX_FLT_MAX,BMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts2(capPnts,sepAxis,BMin,BMax);
				BMin -= capsuleRadius;
				BMax += capsuleRadius;
				
				CHECK_SAT(sepAxis,AMin,AMax,BMin,BMax)
			}
		}

		// 面に厚みがある場合の補助的な判定（面法線 x カプセル軸）
		// 交差するかしないかだけを判定
		if(thickness > SCE_PFX_THICKNESS_THRESHOLD) {
			// 厚み側面の法線
			for(int i=0;i<3;i++) {
				// 分離平面
				PfxPlane plane(sideNml[i],facetPnts[i]);

				// カプセルを分離軸に投影して範囲を取得
				PfxFloat test1,test2,BMin;
				test1 = plane.onPlane(capP0);
				test2 = plane.onPlane(capP1);
				BMin = SCE_PFX_MIN(test1,test2) - capsuleRadius;

				if(BMin > 0.0f) {
					return false;
				}
			}

			// ２つの厚み側面のなすエッジ3×カプセル軸
			for(int e=0;e<3;e++) {
				PfxVector3 edgeVec = normalize(cross(sideNml[(e+1)%3],sideNml[e]));
				PfxVector3 capVec = capP1-capP0;

				// エッジが平行であれば判定しない
				if(pfxIsSameDirection(edgeVec,capVec)) continue;

				PfxVector3 sepAxis = normalize(cross(edgeVec,capVec));

				// Trianglesを分離軸に投影して範囲を取得
				PfxFloat triMin,triMax;
				pfxGetProjAxisPnts3(facetPnts,sepAxis,triMin,triMax);

				// カプセルを分離軸に投影して範囲を取得
				PfxFloat BMin=SCE_PFX_FLT_MAX,BMax=-SCE_PFX_FLT_MAX;
				pfxGetProjAxisPnts2(capPnts,sepAxis,BMin,BMax);
				BMin -= capsuleRadius;
				BMax += capsuleRadius;
				
				if(triMax < BMin || BMax < triMin) {
					return false;
				}
			}
		}
	}
	
	//-------------------------------------------
	// ２．衝突点の探索
	
	{
		// 分離軸方向に引き離す(最近接を判定するため、交差回避させる)
		PfxVector3 sepAxis = 1.1f * pfxAbsf(distMin) * axisMin;
		
		const PfxVector3 facetPnts[3] = {
			p0 + sepAxis,
			p1 + sepAxis,
			p2 + sepAxis,
		};

		//--------------------------------------------------------------------
		// 衝突点の探索

		PfxClosestPoints cp;
		PfxVector3 sA,sB;

		//--------------------------------------------------------------------
		//Triangleの頂点 -> Capsule

		// カプセルの線分と面のエッジx3の最近接点の算出
#ifdef SCE_PFX_USE_GEOMETRY
		PfxGeomSegment segmentC((PfxPoint3)capP0,(PfxPoint3)capP1);

		{
			PfxGeomSegment segmentF((PfxPoint3)facetPnts[0],(PfxPoint3)facetPnts[1]);
			PfxPoint3 closestPointA,closestPointB;
			PfxFloatInVec sqrDist = closestPoints(segmentC,segmentF,closestPointB,closestPointA);
			cp.add(closestPointA,closestPointB + normalize(closestPointA-closestPointB)*capsuleRadius,sqrDist);
		}
		{
			PfxGeomSegment segmentF((PfxPoint3)facetPnts[1],(PfxPoint3)facetPnts[2]);
			PfxPoint3 closestPointA,closestPointB;
			PfxFloatInVec sqrDist = closestPoints(segmentC,segmentF,closestPointB,closestPointA);
			cp.add(closestPointA,closestPointB + normalize(closestPointA-closestPointB)*capsuleRadius,sqrDist);
		}
		{
			PfxGeomSegment segmentF((PfxPoint3)facetPnts[2],(PfxPoint3)facetPnts[0]);
			PfxPoint3 closestPointA,closestPointB;
			PfxFloatInVec sqrDist = closestPoints(segmentC,segmentF,closestPointB,closestPointA);
			cp.add(closestPointA,closestPointB + normalize(closestPointA-closestPointB)*capsuleRadius,sqrDist);
		}
#else
		pfxClosestTwoLines(capP0,capP1,facetPnts[0],facetPnts[1],sB,sA);
		cp.add(PfxPoint3(sA),PfxPoint3(sB + normalize(sA-sB)*capsuleRadius),lengthSqr(sA-sB));
		
		pfxClosestTwoLines(capP0,capP1,facetPnts[1],facetPnts[2],sB,sA);
		cp.add(PfxPoint3(sA),PfxPoint3(sB + normalize(sA-sB)*capsuleRadius),lengthSqr(sA-sB));

		pfxClosestTwoLines(capP0,capP1,facetPnts[2],facetPnts[0],sB,sA);
		cp.add(PfxPoint3(sA),PfxPoint3(sB + normalize(sA-sB)*capsuleRadius),lengthSqr(sA-sB));
#endif

		// カプセルの端点と面の最近接点の算出
		PfxTriangle triangleA(facetPnts[0],facetPnts[1],facetPnts[2]);
		pfxClosestPointTriangle(capP0,triangleA,sA);
		cp.add(PfxPoint3(sA),PfxPoint3(capP0+normalize(sA-capP0)*capsuleRadius),lengthSqr(sA-capP0));

		pfxClosestPointTriangle(capP1,triangleA,sA);
		cp.add(PfxPoint3(sA),PfxPoint3(capP1+normalize(sA-capP1)*capsuleRadius),lengthSqr(sA-capP1));

		for(int i=0;i<cp.numPoints;i++) {
			if(cp.distSqr[i] < cp.closestDistSqr + epsilon) {
				cp.pA[i] -= sepAxis;
				// 面上の最近接点が凸エッジ上でない場合は法線を変える
				if( (((edgeChk&0x03)==0)&&pfxPointOnLine(PfxVector3(cp.pA[i]),p0,p1)) ||
					(((edgeChk&0x0c)==0)&&pfxPointOnLine(PfxVector3(cp.pA[i]),p1,p2)) ||
					(((edgeChk&0x30)==0)&&pfxPointOnLine(PfxVector3(cp.pA[i]),p2,p0)) ) {
					axisMin=-normal;
				}

				PfxSubData subData;
				subData.setFacetId(facetId);
				contacts.addContactPoint(-length(cp.pB[i]-cp.pA[i]),axisMin,cp.pA[i],cp.pB[i],subData);
			}
		}
	}

	return true;
}

PfxInt32 pfxContactTriMeshCapsule(
	PfxContactCache &contacts,
	const PfxLargeTriMesh *largeMeshA,const PfxQuantizedTriMesh *meshA,
	const PfxCapsule &capsuleB,
	const PfxTransform3 &transformB,
	PfxFloat distanceThreshold)
{
	(void) distanceThreshold;

	//-------------------------------------------
	// 判定する面を絞り込む.

	PfxUInt8 SCE_PFX_ALIGNED(16) selFacets[SCE_PFX_NUMMESHFACETS] = { 0 };

	PfxVector3 center = transformB.getTranslation();
	PfxVector3 half = absPerElem(transformB.getUpper3x3()) * PfxVector3(capsuleB.m_halfLen+capsuleB.m_radius);
	PfxQuantize3 quantizedMinHalf = pfxHalve(largeMeshA->quantizePosition(center - half));
	PfxQuantize3 quantizedMaxHalf = pfxHalve(largeMeshA->quantizePosition(center + half));
	PfxUInt32 numSelFacets = pfxGatherFacets(meshA,quantizedMinHalf+quantizedMaxHalf,quantizedMaxHalf-quantizedMinHalf,selFacets);

	if(numSelFacets == 0) {
		return 0;
	}
	
	//-------------------------------------------
	// 分離軸判定(SAT)
	// ※CapsuleB座標系 (Bローカル)で判定

	PfxVector3 vCapAB[2] = {
		PfxVector3(transformB * PfxPoint3(-capsuleB.m_halfLen, 0.0f, 0.0f)),
		PfxVector3(transformB * PfxPoint3( capsuleB.m_halfLen, 0.0f, 0.0f))
	};
	
	PfxDecodedTriMesh decodedMesh;
	PfxContactCache localContacts;
	
	for(PfxUInt32 f = 0; f < numSelFacets; f++) {
		const PfxQuantizedFacet &facet = meshA->m_facets[selFacets[f]];

		// デコード
		PfxDecodedFacet decodedFacet;
		const PfxUInt32 vId[3] = {facet.m_vertIds[0],facet.m_vertIds[1],facet.m_vertIds[2]};

		decodedFacet.m_normal = largeMeshA->decodeNormal(facet.m_normal);
		decodedFacet.m_thickness = largeMeshA->decodeFloat(facet.m_thickness);

	decodedFacet.m_normal = largeMeshA->decodeNormal(facet.m_normal);
	decodedFacet.m_thickness = largeMeshA->decodeFloat(facet.m_thickness);

	for(int v=0;v<3;v++) {
		if(!decodedMesh.isDecodedVertex(vId[v])) {
			decodedMesh.m_verts[vId[v]] = largeMeshA->decodePosition(meshA->m_verts[vId[v]]);
			decodedMesh.m_decodedVertex[vId[v]>>5] |= 1 << (vId[v]&0x1f);
		}
	}

		PfxVector3 facetNormal = decodedFacet.m_normal;

		PfxVector3 facetPntsA[3] = {
			decodedMesh.m_verts[facet.m_vertIds[0]],
			decodedMesh.m_verts[facet.m_vertIds[1]],
			decodedMesh.m_verts[facet.m_vertIds[2]],
		};

		const PfxEdge *edge[3] = {
			&meshA->m_edges[facet.m_edgeIds[0]],
			&meshA->m_edges[facet.m_edgeIds[1]],
			&meshA->m_edges[facet.m_edgeIds[2]],
		};
		
		PfxUInt32 edgeChk = edge[0]->m_angleType | (edge[1]->m_angleType << 2) | (edge[2]->m_angleType << 4);

		pfxContactTriangleCapsule(localContacts,selFacets[f],
							facetNormal,facetPntsA[0],facetPntsA[1],facetPntsA[2],
							decodedFacet.m_thickness,
							0.5f*SCE_PFX_PI*(edge[0]->m_tilt/255.0f),
							0.5f*SCE_PFX_PI*(edge[1]->m_tilt/255.0f),
							0.5f*SCE_PFX_PI*(edge[2]->m_tilt/255.0f),
							edgeChk,capsuleB.m_radius,vCapAB[0],vCapAB[1]);
	}
	
	for(int i=0;i<localContacts.getNumContacts();i++) {
		PfxSubData subData = localContacts.getSubData(i);
		
		const PfxQuantizedFacet &facet = meshA->m_facets[subData.getFacetId()];
		
		PfxTriangle triangleA(
			decodedMesh.m_verts[facet.m_vertIds[0]],
			decodedMesh.m_verts[facet.m_vertIds[1]],
			decodedMesh.m_verts[facet.m_vertIds[2]]);

		PfxFloat s=0.0f,t=0.0f;
		pfxGetLocalCoords(PfxVector3(localContacts.getLocalPointA(i)),triangleA,s,t);
		subData.m_type = PfxSubData::MESH_INFO;
		subData.setFacetLocalS(s);
		subData.setFacetLocalT(t);
		subData.setUserData(facet.m_userData);
		
		contacts.addContactPoint(
			localContacts.getDistance(i),
			localContacts.getNormal(i),
			localContacts.getLocalPointA(i),
			localContacts.getLocalPointB(i),
			subData);
	}
	
	return contacts.getNumContacts();
}

PfxInt32 pfxContactTriMeshCapsule(
	PfxContactCache &contacts,
	const PfxLargeTriMesh *largeMeshA,const PfxExpandedTriMesh *meshA,
	const PfxCapsule &capsuleB,
	const PfxTransform3 &transformB,
	PfxFloat distanceThreshold)
{
	(void) largeMeshA;
	(void) distanceThreshold;

	//-------------------------------------------
	// 判定する面を絞り込む.

	PfxUInt8 SCE_PFX_ALIGNED(16) selFacets[SCE_PFX_NUMMESHFACETS] = { 0 };
	PfxUInt32 numSelFacets = pfxGatherFacets(meshA,
		transformB.getTranslation(),
		absPerElem(transformB.getUpper3x3()) * PfxVector3(capsuleB.m_halfLen+capsuleB.m_radius),selFacets);

	if(numSelFacets == 0) {
		return 0;
	}
	
	//-------------------------------------------
	// 分離軸判定(SAT)

	PfxVector3 vCapAB[2] = {
		PfxVector3(transformB * PfxPoint3(-capsuleB.m_halfLen, 0.0f, 0.0f)),
		PfxVector3(transformB * PfxPoint3( capsuleB.m_halfLen, 0.0f, 0.0f))
	};
	
	PfxContactCache localContacts;
	
	for(PfxUInt32 f = 0; f < numSelFacets; f++) {
		const PfxExpandedFacet &facet = meshA->m_facets[selFacets[f]];

		PfxVector3 facetNormal = pfxReadVector3((PfxFloat*)&facet.m_normal);

		PfxVector3 facetPntsA[3] = {
			meshA->m_verts[facet.m_vertIds[0]],
			meshA->m_verts[facet.m_vertIds[1]],
			meshA->m_verts[facet.m_vertIds[2]],
		};

		const PfxEdge *edge[3] = {
			&meshA->m_edges[facet.m_edgeIds[0]],
			&meshA->m_edges[facet.m_edgeIds[1]],
			&meshA->m_edges[facet.m_edgeIds[2]],
		};
		
		PfxUInt32 edgeChk = edge[0]->m_angleType | (edge[1]->m_angleType << 2) | (edge[2]->m_angleType << 4);

		pfxContactTriangleCapsule(localContacts,selFacets[f],
							facetNormal,facetPntsA[0],facetPntsA[1],facetPntsA[2],
							facet.m_thickness,
							0.5f*SCE_PFX_PI*(edge[0]->m_tilt/255.0f),
							0.5f*SCE_PFX_PI*(edge[1]->m_tilt/255.0f),
							0.5f*SCE_PFX_PI*(edge[2]->m_tilt/255.0f),
							edgeChk,capsuleB.m_radius,vCapAB[0],vCapAB[1]);
	}
	
	for(int i=0;i<localContacts.getNumContacts();i++) {
		PfxSubData subData = localContacts.getSubData(i);
		
		const PfxExpandedFacet &facet = meshA->m_facets[subData.getFacetId()];
		
		PfxTriangle triangleA(
			meshA->m_verts[facet.m_vertIds[0]],
			meshA->m_verts[facet.m_vertIds[1]],
			meshA->m_verts[facet.m_vertIds[2]]);
		
		PfxFloat s=0.0f,t=0.0f;
		pfxGetLocalCoords(PfxVector3(localContacts.getLocalPointA(i)),triangleA,s,t);
		subData.m_type = PfxSubData::MESH_INFO;
		subData.setFacetLocalS(s);
		subData.setFacetLocalT(t);
		subData.setUserData(facet.m_userData);
		
		contacts.addContactPoint(
			localContacts.getDistance(i),
			localContacts.getNormal(i),
			localContacts.getLocalPointA(i),
			localContacts.getLocalPointB(i),
			subData);
	}
	
	return contacts.getNumContacts();
}


PfxInt32 pfxContactTriMeshCapsule(
	PfxContactCache &contacts,
	const PfxLargeTriMesh *largeMeshA,const PfxQuantizedTriMeshBvh *meshA,
	const PfxCapsule &capsuleB,
	const PfxTransform3 &transformB,
	PfxFloat distanceThreshold )
{
	(void) distanceThreshold;

	//	building aabb of the capsule within A local
	PfxVector3 _aabbB(capsuleB.m_halfLen+capsuleB.m_radius,capsuleB.m_radius,capsuleB.m_radius);

	PfxVecInt3 aabbMinL,aabbMaxL;

	largeMeshA->getLocalPosition(
		(transformB.getTranslation() - absPerElem(transformB.getUpper3x3()) * _aabbB),
		(transformB.getTranslation() + absPerElem(transformB.getUpper3x3()) * _aabbB),
		aabbMinL,aabbMaxL);

	PfxAabb16 aabbB;

	pfxSetXMin(aabbB,aabbMinL.getX());
	pfxSetXMax(aabbB,aabbMaxL.getX());
	pfxSetYMin(aabbB,aabbMinL.getY());
	pfxSetYMax(aabbB,aabbMaxL.getY());
	pfxSetZMin(aabbB,aabbMinL.getZ());
	pfxSetZMax(aabbB,aabbMaxL.getZ());

	//	operate back-tracking through Largemsh's BV-tree
	PfxUInt8 SCE_PFX_ALIGNED(16) selFacets[SCE_PFX_NUMMESHFACETS] = {0};
	PfxUInt32 numSelFacets = pfxGatherFacets(meshA,aabbB,selFacets);

	//-------------------------------------------
	//	operating "separating hyperplane theorem"

	PfxDecodedTriMesh decodedMesh;
	PfxContactCache localContacts;

	PfxVector3 vCapAB[2] = {
		PfxVector3(transformB * PfxPoint3(-capsuleB.m_halfLen, 0.0f, 0.0f)),
		PfxVector3(transformB * PfxPoint3( capsuleB.m_halfLen, 0.0f, 0.0f))
	};

	for(PfxUInt32 f = 0; f < numSelFacets; f++ ) {
		const PfxQuantizedFacetBvh &facet = meshA->m_facets[selFacets[f]];
		
		// デコード
		PfxDecodedFacet decodedFacet;
		const PfxUInt32 vId[3] = {facet.m_vertIds[0],facet.m_vertIds[1],facet.m_vertIds[2]};

		decodedFacet.m_normal = largeMeshA->decodeNormal(facet.m_normal);
		decodedFacet.m_thickness = largeMeshA->decodeFloat(facet.m_thickness);

		decodedFacet.m_normal = largeMeshA->decodeNormal(facet.m_normal);
		decodedFacet.m_thickness = largeMeshA->decodeFloat(facet.m_thickness);

		for(int v=0;v<3;v++) {
			if(!decodedMesh.isDecodedVertex(vId[v])) {
				decodedMesh.m_verts[vId[v]] = largeMeshA->decodePosition(meshA->m_verts[vId[v]]);
				decodedMesh.m_decodedVertex[vId[v]>>5] |= 1 << (vId[v]&0x1f);
			}
		}

		PfxVector3 facetNormal = decodedFacet.m_normal;
		
		PfxVector3 facetPntsA[3] = {
			decodedMesh.m_verts[facet.m_vertIds[0]],
			decodedMesh.m_verts[facet.m_vertIds[1]],
			decodedMesh.m_verts[facet.m_vertIds[2]],
		};
		
		const PfxEdge *edge[3] = {
			&meshA->m_edges[facet.m_edgeIds[0]],
			&meshA->m_edges[facet.m_edgeIds[1]],
			&meshA->m_edges[facet.m_edgeIds[2]],
		};
		
		PfxUInt32 edgeChk = edge[0]->m_angleType | (edge[1]->m_angleType << 2) | (edge[2]->m_angleType << 4);

		pfxContactTriangleCapsule(localContacts, selFacets[f],
							facetNormal,facetPntsA[0],facetPntsA[1],facetPntsA[2],
							decodedFacet.m_thickness,
							0.5f*SCE_PFX_PI*(edge[0]->m_tilt/255.0f),
							0.5f*SCE_PFX_PI*(edge[1]->m_tilt/255.0f),
							0.5f*SCE_PFX_PI*(edge[2]->m_tilt/255.0f),
							edgeChk,capsuleB.m_radius,vCapAB[0],vCapAB[1]);
	}

	for(int i=0;i<localContacts.getNumContacts();i++)
	{
		PfxSubData subData = localContacts.getSubData(i);

		const PfxQuantizedFacetBvh &facet = meshA->m_facets[subData.getFacetId()];

		PfxTriangle triangleA(
			decodedMesh.m_verts[facet.m_vertIds[0]],
			decodedMesh.m_verts[facet.m_vertIds[1]],
			decodedMesh.m_verts[facet.m_vertIds[2]]);

		PfxFloat s=0.0f,t=0.0f;
		pfxGetLocalCoords(PfxVector3(localContacts.getLocalPointA(i)),triangleA,s,t);
		subData.m_type = PfxSubData::MESH_INFO;
		subData.setFacetLocalS(s);
		subData.setFacetLocalT(t);
		subData.setUserData(facet.m_userData);
		
		contacts.addContactPoint(
			localContacts.getDistance(i),
			localContacts.getNormal(i),
			localContacts.getLocalPointA(i),
			localContacts.getLocalPointB(i),
			subData);
	}

	return contacts.getNumContacts();
}

PfxInt32 pfxContactTriMeshCapsule(
	PfxContactCache &contacts,
	const PfxLargeTriMesh *largeMeshA,const PfxExpandedTriMeshBvh *meshA,
	const PfxCapsule &capsuleB,
	const PfxTransform3 &transformB,
	PfxFloat distanceThreshold )
{
	(void) distanceThreshold;

	//	building aabb of the capsule within A local
	PfxVector3 _aabbB(capsuleB.m_halfLen+capsuleB.m_radius,capsuleB.m_radius,capsuleB.m_radius);

	PfxVecInt3 aabbMinL,aabbMaxL;

	largeMeshA->getLocalPosition(
		(transformB.getTranslation() - absPerElem(transformB.getUpper3x3()) * _aabbB),
		(transformB.getTranslation() + absPerElem(transformB.getUpper3x3()) * _aabbB),
		aabbMinL,aabbMaxL);

	PfxAabb16 aabbB;

	pfxSetXMin(aabbB,aabbMinL.getX());
	pfxSetXMax(aabbB,aabbMaxL.getX());
	pfxSetYMin(aabbB,aabbMinL.getY());
	pfxSetYMax(aabbB,aabbMaxL.getY());
	pfxSetZMin(aabbB,aabbMinL.getZ());
	pfxSetZMax(aabbB,aabbMaxL.getZ());

	//	operate back-tracking through Largemsh's BV-tree
	PfxUInt8 SCE_PFX_ALIGNED(16) selFacets[SCE_PFX_NUMMESHFACETS] = {0};
	PfxUInt32 numSelFacets = pfxGatherFacets(meshA,aabbB,selFacets);

	//-------------------------------------------
	//	operating "separating hyperplane theorem"

	PfxVector3 vCapAB[2] = {
		PfxVector3(transformB * PfxPoint3(-capsuleB.m_halfLen, 0.0f, 0.0f)),
		PfxVector3(transformB * PfxPoint3( capsuleB.m_halfLen, 0.0f, 0.0f))
	};

	PfxDecodedTriMesh decodedMesh;
	PfxContactCache localContacts;

	for(PfxUInt32 f = 0; f < numSelFacets; f++ ) {
		const PfxExpandedFacetBvh &facet = meshA->m_facets[selFacets[f]];
	
		PfxVector3 facetNormal = pfxReadVector3((PfxFloat*)&facet.m_normal);
		
		PfxVector3 facetPntsA[3] = {
			meshA->m_verts[facet.m_vertIds[0]],
			meshA->m_verts[facet.m_vertIds[1]],
			meshA->m_verts[facet.m_vertIds[2]],
		};
		
		const PfxEdge *edge[3] = {
			&meshA->m_edges[facet.m_edgeIds[0]],
			&meshA->m_edges[facet.m_edgeIds[1]],
			&meshA->m_edges[facet.m_edgeIds[2]],
		};
		
		PfxUInt32 edgeChk = edge[0]->m_angleType | (edge[1]->m_angleType << 2) | (edge[2]->m_angleType << 4);

		pfxContactTriangleCapsule(localContacts, selFacets[f],
							facetNormal,facetPntsA[0],facetPntsA[1],facetPntsA[2],
							facet.m_thickness,
							0.5f*SCE_PFX_PI*(edge[0]->m_tilt/255.0f),
							0.5f*SCE_PFX_PI*(edge[1]->m_tilt/255.0f),
							0.5f*SCE_PFX_PI*(edge[2]->m_tilt/255.0f),
							edgeChk,capsuleB.m_radius,vCapAB[0],vCapAB[1]);
	}

	for(int i=0;i<localContacts.getNumContacts();i++)
	{
		PfxSubData subData = localContacts.getSubData(i);

		const PfxExpandedFacetBvh &facet = meshA->m_facets[subData.getFacetId()];

		PfxTriangle triangleA(
			meshA->m_verts[facet.m_vertIds[0]],
			meshA->m_verts[facet.m_vertIds[1]],
			meshA->m_verts[facet.m_vertIds[2]]);

		PfxFloat s=0.0f,t=0.0f;
		pfxGetLocalCoords(PfxVector3(localContacts.getLocalPointA(i)),triangleA,s,t);
		subData.m_type = PfxSubData::MESH_INFO;
		subData.setFacetLocalS(s);
		subData.setFacetLocalT(t);
		subData.setUserData(facet.m_userData);
		
		contacts.addContactPoint(
			localContacts.getDistance(i),
			localContacts.getNormal(i),
			localContacts.getLocalPointA(i),
			localContacts.getLocalPointB(i),
			subData);
	}

	return contacts.getNumContacts();
}


} //namespace PhysicsEffects
} //namespace sce
