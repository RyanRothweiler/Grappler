using UnityEngine;
using System.Collections;

public class HealAvoid : MonoBehaviour 
{

	public GameObject healer;
	public GameObject avoider;

	void Start () 
	{
		GameObject healerObj = GameObject.Instantiate(healer, this.transform.position, Quaternion.identity) as GameObject;
		GameObject avoiderObj = GameObject.Instantiate(avoider, this.transform.position, Quaternion.identity) as GameObject;
		healerObj.GetComponent<Healer>().objAttached = avoiderObj;
	}
}
