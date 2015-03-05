using UnityEngine;
using System.Collections;

public class HealAvoid : MonoBehaviour 
{

	public GameObject healer;
	public GameObject avoider;

	void Start () 
	{
		Vector3 variablePos;
		variablePos = new Vector3(this.transform.position.x + Random.Range(-1, 1), 
		                          this.transform.position.y + Random.Range(-1, 1), 
		                          this.transform.position.z + Random.Range(-1, 1));
		GameObject healerObj = GameObject.Instantiate(healer, variablePos, Quaternion.identity) as GameObject;
		variablePos = new Vector3(this.transform.position.x + Random.Range(-1, 1), 
		                          this.transform.position.y + Random.Range(-1, 1), 
		                          this.transform.position.z + Random.Range(-1, 1));
		GameObject avoiderObj = GameObject.Instantiate(avoider, variablePos, Quaternion.identity) as GameObject;
		healerObj.GetComponent<Healer>().objAttached = avoiderObj;
	}
}
