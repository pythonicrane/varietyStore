using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CreateMeshCollider : MonoBehaviour {

    private List<Vector3> pointCloud;
    private List<int> indexCloud;
    private GameObject bodyIndex;
    private SkinnedMeshRenderer _skinnedMeshRenderer;
    private Mesh _mesh;

    // Use this for initialization
    void Start()
    {
        bodyIndex = GameObject.Find("m_avg");
        //_mesh = bodyIndex.GetComponent<SkinnedMeshRenderer>().sharedMesh;
        _mesh = Instantiate(bodyIndex.GetComponent<SkinnedMeshRenderer>().sharedMesh) as Mesh;
        _skinnedMeshRenderer = bodyIndex.GetComponent<SkinnedMeshRenderer>();

        pointCloud = new List<Vector3>();
        indexCloud = new List<int>();
        for (int i = 0; i < _mesh.vertexCount; ++i)
        {
            if (_mesh.boneWeights[i].boneIndex0 ==1)
            {
                indexCloud.Add(i);
            }
        }

    }

    // Update is called once per frame
    void Update()
    {
        Mesh meshUpdate = new Mesh();
        _skinnedMeshRenderer.BakeMesh(meshUpdate);
        if (Input.GetKeyDown(KeyCode.C))
        {
            Debug.Log(meshUpdate.vertices[0]);

        }


        pointCloud.Clear();
        for (int i = 0; i < indexCloud.Count; ++i)
        {
            //Vector3 _point = _mesh.vertices[indexCloud[i]];
            pointCloud.Add(meshUpdate.vertices[indexCloud[i]]);
        }



        if (pointCloud != null)
        {
            Mesh tempMesh = new Mesh();
            CreateMesh(ref tempMesh, ref pointCloud, pointCloud.Count);
            this.GetComponent<MeshCollider>().sharedMesh = tempMesh;
            this.GetComponent<MeshCollider>().convex = true;
        }


    }

    void CreateMesh(ref Mesh mesh, ref List<Vector3> arrayListXYZ, int pointsNum)
    {
        Vector3[] points    = new Vector3[pointsNum];
        int[] indecies      = new int[pointsNum];
        int[] triangles     = new int[(pointsNum - 2) * 3];
        for (int i = 0; i < pointsNum; ++i)
        {
            points[i] = arrayListXYZ[i];
            indecies[i] = i;
            if (i < pointsNum - 2)
            {
                triangles[i * 3] = i;
                triangles[i * 3 + 1] = i + 1;
                triangles[i * 3 + 2] = i + 2;
            }
        }

        mesh.vertices = points;
        mesh.SetIndices(indecies, MeshTopology.Points, 0);
        mesh.triangles = triangles;

    }
}
