using UnityEngine;
using System.Collections;

public class Capturer : MonoBehaviour 
{

	public static Capturer instance;
	private Transform[] allChildren;

	void Start () 
	{
		allChildren = this.gameObject.GetComponentsInChildren<Transform>() as Transform[];

		instance = this;
		Hide();
	}
	
	void Update () 
	{

	}

	public void Show()
	{
		foreach (Transform child in allChildren)
		{
			if (child.gameObject.name == "Capturer")
			{

			}
			else
			{
				child.gameObject.SetActive(true);
			}
		}
	}

	public void Hide()
	{
		foreach (Transform child in allChildren)
		{
			if (child.gameObject.name == "Capturer")
			{
				continue;
			}
			else
			{
				child.gameObject.SetActive(false);
			}
		}
	}

	void OnTriggerEnter2D(Collider2D coll)
	{
		if (!PlayerController.instance.isStunned)
		{
			AIBehavior aiBehavior = coll.gameObject.GetComponent<AIBehavior>();
			if (aiBehavior)
			{
				PlayerController.instance.objCaptured = coll.gameObject;
			}
		}
	}

	void OnTriggerExit2D(Collider2D coll)
	{
		if (!PlayerController.instance.isPulling)
		{
			PlayerController.instance.objCaptured = null;
		}
	}
}
