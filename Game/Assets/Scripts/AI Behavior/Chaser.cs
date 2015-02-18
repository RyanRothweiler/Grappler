using UnityEngine;
using System.Collections;

public class Chaser : MonoBehaviour 
{

	private float movementSpeed = 5f;

	public void Update()
	{
		if (Vector3.Distance(PlayerController.instance.gameObject.transform.position, this.transform.position) < 10)
		{
			PointAt(this.gameObject, PlayerController.instance.gameObject.transform.position);
			this.transform.position += this.transform.up * movementSpeed * 0.01f;
		}
	}

	public void PointAt(GameObject objPointing, Vector3 target)
	{
		Vector3 dir = target - objPointing.transform.position;
		float angle = Mathf.Atan2(dir.y, dir.x) * Mathf.Rad2Deg;
		angle -= 90;
		objPointing.transform.rotation = Quaternion.AngleAxis(angle, Vector3.forward);
	}

}