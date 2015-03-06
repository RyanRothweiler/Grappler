using UnityEngine;
using System.Collections;

public class EnemyBehavior : MonoBehaviour 
{

	public virtual void Start() { }

	public virtual void Update () 
	{
		if (!this.GetComponent<EnemyController>().isStunned)
		{
			Act();
		}
	}

	public virtual void Act() { }
}
