using UnityEngine;
using System.Collections;

public class God : MonoBehaviour 
{
	public static God instance;
	public bool isPaused;
	public bool psControllerLayout;

	public void Start()
	{
		instance = this;
		isPaused = false;
	}
}
