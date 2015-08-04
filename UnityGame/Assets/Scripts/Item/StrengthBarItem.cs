using UnityEngine;
using System.Collections;

public class StrengthBarItem : Item 
{
	public override void Use()
	{
		PlayerController.instance.currentPullStrength += 40;
	}
}