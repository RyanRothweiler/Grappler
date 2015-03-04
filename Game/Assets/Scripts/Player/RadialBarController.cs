using UnityEngine;
using System.Collections;

public class RadialBarController : MonoBehaviour 
{

	void Start () 
	{
		this.GetComponent<Animation>().Play("RadialBarAnim");
	}
	
	void Update () 
	{
		this.transform.position = PlayerController.instance.transform.position;

		float animCoefficient = PlayerController.instance.currentPullStrength / PlayerController.instance.totalPullStrength;
		float currentAnimTime = animCoefficient * this.GetComponent<Animation>()["RadialBarAnim"].length;
		this.GetComponent<Animation>()["RadialBarAnim"].time = currentAnimTime;
		this.GetComponent<Animation>().Play("RadialBarAnim");
	}
}
