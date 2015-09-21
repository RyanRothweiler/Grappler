using UnityEngine;
using System.Collections;

public class HealChase : MonoBehaviour 
{

	public GameObject healer;
	public GameObject chase;

	void Start () 
	{
		GameObject healerObj = GameObject.Instantiate(healer, this.transform.position, Quaternion.identity) as GameObject;
		GameObject chaseObj = GameObject.Instantiate(chase, this.transform.position, Quaternion.identity) as GameObject;
		healerObj.GetComponent<Healer>().objAttached = chaseObj;
	}
}
