using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Obi;

public class clothCreate : MonoBehaviour {

    private Vector3 attachmentOffset;

    private ObiCloth cloth;//ObiCloth组件
    private ObiSolver solver;//计算器组件
    private ObiMeshTopology topology;//mesh拓扑生成器

    private GameObject myPlane;//平面布料
    private GameObject myCube;//方形碰撞体
    private GameObject mySolver;//求解器


    // Use this for initialization
    public IEnumerator Start()
    {
        attachmentOffset = new Vector3(2, 0, 0);

        yield return MakeCloth(new Vector3(2,0,0));

        
    }

    // Update is called once per frame
    void Update()
    {

    }

    public IEnumerator MakeCloth(Vector3 attachmentOffset)//Transform anchoredTo, Vector3 attachmentOffset
    {
        /*生成方形碰撞体*/
        myCube = GameObject.CreatePrimitive(PrimitiveType.Cube);
        myCube.name = "myCube";
        myCube.transform.position = attachmentOffset;

        /*生成平面布料*/
        myPlane = GameObject.CreatePrimitive(PrimitiveType.Plane);
        myPlane.name = "myPlane";
        myPlane.AddComponent<ObiCloth>();
        myPlane.transform.localScale = new Vector3(0.3f, 0.3f, 0.3f);
        myPlane.transform.position = new Vector3(attachmentOffset.x, attachmentOffset.y + 2, attachmentOffset.z);
        

        /*生成求解器*/
        mySolver = new GameObject("mySolver", typeof(ObiSolver));


        /*各组件绑定*/
        cloth = myPlane.GetComponent<ObiCloth>();
        solver = mySolver.GetComponent<ObiSolver>();

        /*生成拓扑*/
        topology = ScriptableObject.CreateInstance<ObiMeshTopology>();
        topology.InputMesh = myPlane.GetComponent<MeshCollider>().sharedMesh;
        topology.scale = new Vector3(0.3f, 0.3f, 0.3f);
        topology.Generate();

        /*衣服组件绑定求解器和拓扑*/
        cloth.Solver = solver;
        cloth.SharedTopology = topology;

        //不需要Init？

        //碰撞处理
        addCollision();

        //多线程
        yield return cloth.StartCoroutine(cloth.GeneratePhysicRepresentationForMesh());
        cloth.AddToSolver(null);

    }

    public void addCollision()
    {
        myPlane.layer = 6;
        cloth.SelfCollisions = true;

        ObiColliderGroup myColliderGroup;
        myColliderGroup = mySolver.AddComponent<ObiColliderGroup>();
        myColliderGroup.AddCollider(myCube.GetComponent<BoxCollider>());
        myColliderGroup.AddCollider(GameObject.Find("TestEnvironment").GetComponent<MeshCollider>());

        solver.colliderGroup = myColliderGroup;
    }
}
