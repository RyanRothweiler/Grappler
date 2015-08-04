using UnityEngine;
using System.Collections;

public class LadderController : MonoBehaviour 
{
	public bool canEnter;
	public TextMesh textMesh;

	void Update () 
	{
		// EnemyController[] enemies = Object.FindObjectsOfType<EnemyController>();
		// canEnter = true;
		// int enemyCount = 0;
		// foreach(EnemyController enemy in enemies)
		// {
		// 	if (enemy.alive)
		// 	{
		// 		enemyCount++;
		// 		canEnter = false;
		// 	}
		// }

		// if (canEnter)
		// {
		// 	textMesh.text = "";
		// }
		// else
		// {
		// 	textMesh.text = enemyCount + "enemies left";
		// }
	}
}
