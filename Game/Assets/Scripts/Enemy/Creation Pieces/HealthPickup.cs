using UnityEngine;
using System.Collections;

public class HealthPickup : MonoBehaviour 
{

	public GameObject heartFab;
	public GameObject shooterFab;

	void Start () 
	{
		Vector3 variablePos;
		variablePos = new Vector3(this.transform.position.x + Random.Range(-1, 1), 
		                          this.transform.position.y + Random.Range(-1, 1), 
		                          this.transform.position.z + Random.Range(-1, 1));
		GameObject.Instantiate(heartFab, variablePos, Quaternion.identity);
		variablePos = new Vector3(this.transform.position.x + Random.Range(-1, 1), 
		                          this.transform.position.y + Random.Range(-1, 1), 
		                          this.transform.position.z + Random.Range(-1, 1));
		GameObject.Instantiate(shooterFab, this.transform.position, Quaternion.identity);
		variablePos = new Vector3(this.transform.position.x + Random.Range(-1, 1), 
		                          this.transform.position.y + Random.Range(-1, 1), 
		                          this.transform.position.z + Random.Range(-1, 1));
		GameObject.Instantiate(shooterFab, this.transform.position, Quaternion.identity);		
	}
}