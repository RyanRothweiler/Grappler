using UnityEngine;
using System.Collections;

public class DeathTimer : MonoBehaviour 
{

	void Start () 
	{
		StartCoroutine(Death());
	}

	public IEnumerator Death()
	{
		yield return new WaitForSeconds(1f);
		Destroy(this.gameObject);
	}
}
