using UnityEngine;
using System.Collections;

public class Shooter : EnemyBehavior 
{

	public GameObject bulletObj;
	public bool canShoot;
	public float relaodTime;

	public GameObject currentSpawnBar;

	public float lastTimeSpawned;
	public float nextSpawnTime;

	
	public override void Act () 
	{
		base.Act();
		
		float playerDist = Vector3.Distance(this.transform.position, PlayerController.instance.transform.position);
		if (playerDist < 10)
		{
			if (this.GetComponent<EnemyController>().alive && !this.GetComponent<EnemyController>().isRunning)
			{
				if (canShoot)
				{
					this.GetComponent<Rigidbody2D>().angularVelocity = 0;;

					lastTimeSpawned = Time.time;
					nextSpawnTime = Time.time + relaodTime;
					Utility.PointAt(this.gameObject, PlayerController.instance.transform.position);
					GameObject.Instantiate(bulletObj, this.transform.position, Quaternion.identity);
					canShoot = false;
					StartCoroutine(ResetCanShoot());
				}
			}
		}

		float xScale = Mathf.Clamp((Time.time - lastTimeSpawned) / (nextSpawnTime - lastTimeSpawned), 0, 1);
		Vector3 newScale = new Vector3(xScale, 
		                               currentSpawnBar.transform.localScale.y, 
		                               currentSpawnBar.transform.localScale.z);
		currentSpawnBar.transform.localScale = newScale;
	}

	public IEnumerator ResetCanShoot()
	{
		yield return new WaitForSeconds(relaodTime);
		canShoot = true;
	}
}
