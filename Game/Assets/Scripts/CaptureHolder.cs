using UnityEngine;
using System.Collections;

public class CaptureHolder : MonoBehaviour 
{

	public static CaptureHolder instance;

	void Start () 
	{
		instance = this;		
	}
	
	void Update () 
	{
		if (PlayerController.instance.isPulling)
		{
			this.GetComponent<SpringJoint2D>().enabled = false;

			if (Vector3.Distance(PlayerController.instance.objCaptured.transform.position, this.transform.position) < 1.2f)
			{
				Destroy(PlayerController.instance.objCaptured);
				PlayerController.instance.KillPull();
				// PlayerController.instance.gameObject.GetComponent<SpringJoint2D>().enabled = false;
				// objCaptured.transform.position = new Vector3(1000, 1000, 1000);
			}
		}
		else
		{
			this.GetComponent<SpringJoint2D>().enabled = true;
		}
	}
}
