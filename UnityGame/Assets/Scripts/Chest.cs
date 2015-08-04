using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class Chest : MonoBehaviour 
{

	public bool open;
	public Sprite openSprite;
	public List<GameObject> itemsHolding;

	public void Start()
	{
		open = false;
	}

	public void Open()
	{
		open = true;
		this.GetComponent<SpriteRenderer>().sprite = openSprite;

		foreach (GameObject item in itemsHolding)
		{
			Vector3 pos = new Vector3(this.transform.position.x + Random.Range(-1, 1),
			                          this.transform.position.y + Random.Range(2, 1),
			                          0);
			GameObject.Instantiate(item, pos, Quaternion.identity);
		}
	}
}
