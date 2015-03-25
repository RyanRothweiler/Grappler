using UnityEngine;
using System.Collections;

public class TutorialLadder : MonoBehaviour 
{
	public int sceneIndexTo;
	public bool canEnter;

	public void Update()
	{
		EnemyController[] enemies = Object.FindObjectsOfType<EnemyController>();
		canEnter = true;
		int enemyCount = 0;
		foreach(EnemyController enemy in enemies)
		{
			if (enemy.alive)
			{
				enemyCount++;
				canEnter = false;
			}
		}
	}
}
