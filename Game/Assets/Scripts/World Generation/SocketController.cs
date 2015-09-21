using UnityEngine;
using System.Collections;

public class SocketController : MonoBehaviour 
{
	// either "single" or "double"
	public string socketType;
	public bool isTaken = false;

	public void OnDrawGizmos() 
	{
		Gizmos.color = Color.red;
		Gizmos.DrawLine(this.transform.position, this.transform.position + (this.transform.up * 3));
	}
}
