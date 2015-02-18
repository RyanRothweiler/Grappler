using UnityEngine;
using System.Collections;

public class LittleDudeController : MonoBehaviour 
{

	public float maxHealth;
	public float currentHealth;

	public GameObject healthBar;

	public bool alive;

	void Start () 
	{
		alive = true;
	}
	
	void Update () 
	{
		Vector3 newScale = new Vector3(currentHealth / maxHealth, 1, 1);
		healthBar.transform.localScale = newScale;

		if (currentHealth < 0)
		{
			alive = false;
			this.transform.position = new Vector3(1000, 1000, 1000);
		}
	}
}