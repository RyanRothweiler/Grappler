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
				if (PlayerController.instance.objCaptured == null)
				{
					PlayerController.instance.objCaptured = coll.gameObject;
				}
				else
				{
					float newCollDist = Vector3.Distance(PlayerController.instance.objCaptured.transform.position, PlayerController.instance.gameObject.transform.position);
					float oldCollDist = Vector3.Distance(coll.gameObject.transform.position, PlayerController.instance.gameObject.transform.position);
					if (newCollDist > oldCollDist)
					{
						PlayerController.instance.objCaptured = coll.gameObject;
					}
				}
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
