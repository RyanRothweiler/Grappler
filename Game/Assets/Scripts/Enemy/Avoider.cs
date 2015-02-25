using UnityEngine;
using System.Collections;

public class Avoider : MonoBehaviour 
{
	public void Update()
	{
		float dist = Vector3.Distance(PlayerController.instance.gameObject.transform.position, 
		                              this.gameObject.transform.position);
		if (dist < 5)
		{
			Utility.PointAt(this.gameObject, PlayerController.instance.gameObject.transform.position);

			Rigidbody2D body = this.gameObject.GetComponent<Rigidbody2D>() as Rigidbody2D;
			body.AddForce(-this.transform.up * this.GetComponent<EnemyController>().movementSpeed * 2);
		}
	}
}