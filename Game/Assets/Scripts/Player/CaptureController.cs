using UnityEngine;
using System.Collections;

public class CaptureController : MonoBehaviour 
{

	public static CaptureController instance;

	void Start () 
	{
		instance = this;
	}

	void OnTriggerEnter2D(Collider2D coll)
	{
		if (!PlayerController.instance.isStunned && !PlayerController.instance.isPulling)
		{
			EnemyController enemyController = coll.gameObject.GetComponent<EnemyController>();
			if (enemyController)
			{
				PlayerController.instance.objCaptured = coll.gameObject;
			}
		}
	}

	void OnTriggerExit2D(Collider2D coll)
	{
		if (!PlayerController.instance.isPulling)
		{
			PlayerController.instance.objCaptured = null;
		}
	}
}
