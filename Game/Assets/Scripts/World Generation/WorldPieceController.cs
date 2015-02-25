using UnityEngine;
using System.Collections;

public class WorldPieceController : MonoBehaviour 
{

	public GameObject[] socketObjects;
	public bool collided = false;

	void OnTriggerEnter2D(Collider2D coll)
	{
		collided = true;
	}
}