using UnityEngine;
using System.Collections;

public class PlayerMinimapPos : MonoBehaviour 
{
	
	void Update () 
	{
		Vector3 newPos = PlayerController.instance.transform.position + new Vector3(2000, 2000, 2000);
		this.transform.position = newPos;
	}
}
