using UnityEngine;
using System.Collections;

public class FlyNest : MonoBehaviour 
{

	public GameObject fly;

	void Start () 
	{
		for (int i = 0;
		     i < 5;
		     i++)
		{
			GameObject.Instantiate(fly, this.transform.position, Quaternion.identity);
		}	
	}
}
