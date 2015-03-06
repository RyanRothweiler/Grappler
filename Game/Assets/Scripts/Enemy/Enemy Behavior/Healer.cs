using UnityEngine;
using System.Collections;

public class Healer : EnemyBehavior 
{
	public float healSpeed;
	public GameObject objAttached;
	public LineRenderer healLine;

	public override void Start()
	{
		base.Start();

		this.GetComponent<SpringJoint2D>().connectedBody = objAttached.GetComponent<Rigidbody2D>();
		healLine = this.GetComponent<LineRenderer>();
		objAttached.GetComponent<EnemyController>().objectsAttached.Add(this.gameObject);
	}

	public override void Act () 
	{
		base.Act();

		if (!objAttached || objAttached.transform.position.x == 1000)
		{
			this.healLine.enabled = false;
			this.GetComponent<SpringJoint2D>().enabled = false;
			objAttached = null;
		}

		if (objAttached)
		{
			healLine.SetPosition(0, this.transform.position);
			healLine.SetPosition(1, objAttached.transform.position);

			objAttached.GetComponent<EnemyController>().currentHealth += healSpeed;
		}
	}

	public void Detach()
	{
		this.healLine.enabled = false;
		this.GetComponent<SpringJoint2D>().enabled = false;
		objAttached = null;
	}
}