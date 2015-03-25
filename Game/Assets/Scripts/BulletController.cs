using UnityEngine;
using System.Collections;

public class BulletController : MonoBehaviour 
{

	public bool alive = false;
	public float speed;

	void Start()
	{
		Utility.PointAt(this.gameObject, PlayerController.instance.transform.position);
	}

	
	void Update () 
	{
		if (!God.instance.isPaused)
		{
			Vector3 newPos = this.transform.position + (this.transform.up * speed * 0.28f);
			newPos.z = 0;
			this.transform.position = newPos;
		}
	}

	void OnCollisionEnter2D(Collision2D coll)
	{
		if (alive)
		{
			PlayerController playerCont = coll.gameObject.GetComponent<PlayerController>(); 
			if (playerCont)
			{
				// playerCont.TakeDamage();
			}
			Destroy(this.gameObject);
		}
	}
}