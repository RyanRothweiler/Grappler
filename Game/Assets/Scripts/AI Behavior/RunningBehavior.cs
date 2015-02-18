using UnityEngine;
using System.Collections;

public class RunningBehavior : AIBehavior 
{

	public GameObject objAvoiding;
	public bool isRunning = false;

	public override void RunFromCapture()
	{
		this.objAvoiding = PlayerController.instance.gameObject;
		isRunning = true;
		StartCoroutine(RunFromCapture_());
	}
	private IEnumerator RunFromCapture_()
	{
		Rigidbody2D body = this.gameObject.GetComponent<Rigidbody2D>() as Rigidbody2D;
		while (isRunning)
		{
			PointAt(this.gameObject, PlayerController.instance.gameObject.transform.position);
			if (Random.Range(0, 100) < 70)
			{
				body.AddForce((-this.transform.up * 500) + new Vector3(Random.Range(-0.5f, 0.5f), Random.Range(-0.5f, 0.5f), 0));
			}
			body.AddForce((-this.transform.up * 250) + new Vector3(Random.Range(-0.5f, 0.5f), Random.Range(-0.5f, 0.5f), 0));
			yield return new WaitForSeconds(0.1f);
		}
	}

	public void KillPull()
	{
		isRunning = false;
	}

	public void PointAt(GameObject objPointing, Vector3 target)
	{
		Vector3 dir = target - objPointing.transform.position;
		float angle = Mathf.Atan2(dir.y, dir.x) * Mathf.Rad2Deg;
		angle -= 90;
		objPointing.transform.rotation = Quaternion.AngleAxis(angle, Vector3.forward);
	}
}