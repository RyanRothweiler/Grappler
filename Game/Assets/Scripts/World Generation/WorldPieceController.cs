using UnityEngine;
using System.Collections;

public class WorldPieceController : MonoBehaviour 
{

	public GameObject[] socketObjects;
	public GameObject[] enemySockets;
	public bool collided = false;
	public GameObject miniMapPiece;

	void OnTriggerEnter2D(Collider2D coll)
	{
		collided = true;
	}
}