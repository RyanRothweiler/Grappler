using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class EnemyController : MonoBehaviour 
{

	public bool alive;
	public float maxHealth;
	public float currentHealth;
	public GameObject healthBar;
	public bool isStunned;

	public bool isRunning = false;

	public float movementSpeed;

	public int generatorSpawnSpeed;

	public List<GameObject> objectsAttached;

	void Start () 
	{
		isStunned = false;
		alive = true;
	}
	
	void Update () 
	{
		currentHealth = Mathf.Clamp(currentHealth, -10, maxHealth);
		Vector3 newScale = new Vector3(currentHealth / maxHealth, 1, 1);
		healthBar.transform.localScale = newScale;

		if (currentHealth < 0)
		{
			Healer healer = this.GetComponent<Healer>();
			if (healer)
			{
				healer.objAttached = null;
				healer.GetComponent<SpringJoint2D>().enabled = false;
				healer.healLine.enabled = false;
			}

			foreach (GameObject objAttached in objectsAttached)
			{
				Healer healerAttached = objAttached.GetComponent<Healer>();
				if (healerAttached)
				{
					healerAttached.Detach();
				}
			}

			alive = false;
			this.transform.position = new Vector3(1000, 1000, 1000);
		}
	}

	public void RunFromCapture()
	{
		isRunning = true;
		StartCoroutine(RunFromCapture_());
	}
	private IEnumerator RunFromCapture_()
	{
		Rigidbody2D body = this.gameObject.GetComponent<Rigidbody2D>() as Rigidbody2D;
		while (isRunning)
		{
			if (!isStunned)
			{
				Utility.PointAt(this.gameObject, PlayerController.instance.gameObject.transform.position);
				if (Random.Range(0, 100) < 70)
				{
					body.AddForce((-this.transform.up * 500) + new Vector3(Random.Range(-0.5f, 0.5f), Random.Range(-0.5f, 0.5f), 0));
				}
				body.AddForce((-this.transform.up * 250) + new Vector3(Random.Range(-0.5f, 0.5f), Random.Range(-0.5f, 0.5f), 0));
			}
			yield return new WaitForSeconds(0.1f);
		}
	}

	public void Stun(int length)
	{
		isStunned = true;
		StartCoroutine(Stun_(length));
	}
	private IEnumerator Stun_(int length)
	{
		yield return new WaitForSeconds(length);
		isStunned = false;
	}
}