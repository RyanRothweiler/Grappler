using UnityEngine;
using System.Collections;

public class Chaser : MonoBehaviour 
{
	public void Update()
	{
		if (Vector3.Distance(PlayerController.instance.gameObject.transform.position, this.transform.position) < 10)
		{
			Utility.PointAt(this.gameObject, PlayerController.instance.gameObject.transform.position);

			Rigidbody2D body = this.gameObject.GetComponent<Rigidbody2D>() as Rigidbody2D;
			body.AddForce(this.transform.up * this.GetComponent<EnemyController>().movementSpeed * 2);
		}
	}
}