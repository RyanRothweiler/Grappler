using UnityEngine;
using System.Collections;

public class Generator : MonoBehaviour 
{

	public int spawnTime;
	public bool canSpawn;
	public GameObject objSpawning;
	public GameObject[] objSpawningOptions;
	public GameObject currentSpawnBar;

	public float lastTimeSpawned;
	public float nextSpawnTime;

	void Start()
	{
		if (objSpawning == null)
		{
			objSpawning = objSpawningOptions[Random.Range(0, objSpawningOptions.Length)];
		}
		spawnTime = objSpawning.GetComponent<EnemyController>().generatorSpawnSpeed;
	}

	void Update () 
	{
		float playerDist = Vector3.Distance(this.transform.position, PlayerController.instance.transform.position);
		if (playerDist < 10)
		{
			if (canSpawn)
			{
				canSpawn = false;
				lastTimeSpawned = Time.time;
				nextSpawnTime = Time.time + spawnTime;
				Vector3 spawnPos = new Vector3(this.transform.position.x + Random.Range(-2, 2), 
				                               this.transform.position.y + Random.Range(-2, 2),
				                               this.transform.position.z);
				GameObject.Instantiate(objSpawning, spawnPos, Quaternion.identity);
				StartCoroutine(ResetCanSpawn());
			}

		}

		float newX = Mathf.Clamp((Time.time - lastTimeSpawned) / (nextSpawnTime - lastTimeSpawned), 0, 1);
		Vector3 newScale = new Vector3(newX, 
		                               currentSpawnBar.transform.localScale.y, 
		                               currentSpawnBar.transform.localScale.z);
		currentSpawnBar.transform.localScale = newScale;
	}
	
	public IEnumerator ResetCanSpawn()
	{
		yield return new WaitForSeconds(spawnTime);
		canSpawn = true;
	}
}
