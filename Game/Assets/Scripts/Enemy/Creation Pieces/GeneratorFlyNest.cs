using UnityEngine;
using System.Collections;

public class GeneratorFlyNest : MonoBehaviour 
{

	public GameObject generator;
	public GameObject fly;

	void Start () 
	{
		for (int i = 0;
		     i < 5;
		     i++)
		{
			GameObject.Instantiate(fly, this.transform.position, Quaternion.identity);
		}
		
		GameObject.Instantiate(generator, this.transform.position, Quaternion.identity);
	}
}
