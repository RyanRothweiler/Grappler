using UnityEngine;
using System.Collections;

public class EMPItem : Item 
{

	public GameObject particles;

	public override void Use()
	{
		EnemyController[] enemies = GameObject.FindObjectsOfType<EnemyController>();
		foreach (EnemyController controller in enemies)
		{
			if (Vector3.Distance(PlayerController.instance.transform.position, controller.transform.position) < 5.2f)
			{
				controller.currentHealth -= 20;
			}
		}

		PlayerController.instance.inventory.Remove(this);

		Instantiate(particles, PlayerController.instance.transform.position, Quaternion.identity);

		buttonIn.uiSprite.sprite = null;
		buttonIn.itemHolding = null;
		Destroy(this.gameObject);
	}
}