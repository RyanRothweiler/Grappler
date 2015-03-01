using UnityEngine;
using System.Collections;

public class ShooterGroup : MonoBehaviour 
{

	public GameObject shooter;

	void Start () 
	{
		for (int i = 0;
		     i < 2;
		     i++)
		{
			GameObject.Instantiate(shooter, this.transform.position, Quaternion.identity);
		}	
	}
}
