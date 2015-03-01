using UnityEngine;
using System.Collections;

public class BulletController : MonoBehaviour 
{

	public float speed;

	void Start()
	{
		Utility.PointAt(this.gameObject, PlayerController.instance.transform.position);
	}

	
	void Update () 
	{
		Vector3 newPos = this.transform.position + (this.transform.up * speed * 0.35f);
		newPos.z = 0;
		this.transform.position = newPos;
	}

	void OnCollisionEnter2D(Collision2D coll)
	{
		PlayerController playerCont = coll.gameObject.GetComponent<PlayerController>(); 
		if (playerCont)
		{
			playerCont.TakeDamage();
		}
		Destroy(this.gameObject);
	}
}