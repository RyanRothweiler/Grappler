using UnityEngine;
using System.Collections;

public class StunnerItem : Item 
{

	public GameObject particles;

	public override void Use()
	{
		EnemyController[] enemies = GameObject.FindObjectsOfType<EnemyController>();
		foreach (EnemyController controller in enemies)
		{
			if (Vector3.Distance(PlayerController.instance.transform.position, controller.transform.position) < 5.5f)
			{
				controller.Stun(3);
			}
		}

		Instantiate(particles, PlayerController.instance.transform.position, Quaternion.identity);
	}
}