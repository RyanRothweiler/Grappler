using UnityEngine;
using System.Collections;

public class God : MonoBehaviour 
{
	public static God instance;

	public bool isPaused;

	public void Start()
	{
		instance = this;

		isPaused = false;
	}
}
