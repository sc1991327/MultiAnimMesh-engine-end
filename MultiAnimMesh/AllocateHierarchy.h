//这个类用于被系统调用来加载X文件中的数据到内存按一定的形式存放。

#pragma once

struct D3DXFRAME_DERIVED : public D3DXFRAME
{
	D3DXMATRIX CombinedTransformMatrix;
};

struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER
{
	IDirect3DTexture9**	ppTextures;
	ID3DXMesh*			pOrgMesh;
	DWORD				MaxBonesInflPerVertex;
	DWORD				NumAttrGroups;
	ID3DXBuffer*		pBoneCombBuffer;
	D3DXMATRIX**		ppBoneMatrices;
	D3DXMATRIX**		ppBoneOffsetMatrices;
	DWORD				NumMatrixPalettes;
	bool				UseSoftwareVP;
};


class cAllocateHierarchy : public ID3DXAllocateHierarchy
{
private:
	HRESULT AllocateName(LPCSTR name, LPSTR* ret_name);
	HRESULT GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED* mesh_container);

public:
	STDMETHOD(CreateFrame)(LPCSTR name, LPD3DXFRAME* ret_frame);

	STDMETHOD(CreateMeshContainer)(LPCSTR name, 
                                   CONST D3DXMESHDATA* mesh_data,
                                   CONST D3DXMATERIAL* xmaterials, 
                                   CONST D3DXEFFECTINSTANCE* effect_instances, 
                                   DWORD num_materials, 
                                   CONST DWORD* adjacency, 
                                   LPD3DXSKININFO skin_info, 
                                   LPD3DXMESHCONTAINER* ret_mesh_container);    

    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME frame_to_free);
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER base_mesh_container);

};

