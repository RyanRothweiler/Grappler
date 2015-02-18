using UnityEngine;
using System.Collections;

public class Aimer : MonoBehaviour 
{

	public static Aimer instance;

	void Start () 
	{
		instance = this;
	}
}
