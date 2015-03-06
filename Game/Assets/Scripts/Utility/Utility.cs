using UnityEngine;
using System.Collections;

public static class Utility  
{
	public static void PointAt(GameObject objPointing, Vector3 target)
	{
		Vector3 dir = target - objPointing.transform.position;
		float angle = Mathf.Atan2(dir.y, dir.x) * Mathf.Rad2Deg;
		angle -= 90;
		objPointing.transform.rotation = Quaternion.AngleAxis(angle, Vector3.forward);
	}
}
