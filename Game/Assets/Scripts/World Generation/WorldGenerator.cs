using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class WorldGenerator : MonoBehaviour
{
	public static WorldGenerator instance;

	public GameObject[] pieceLibrary;
	public GameObject singleEnd;
	public GameObject doubleEnd;

	struct slot
	{
		public GameObject piece;
		public GameObject slotObject;
	}

	void Start () 
	{
		instance = this;
		GenerateWorld();
	}
	
	void Update () 
	{

	}

	public GameObject CreatePiece()
	{
		bool looking = true;
		while (looking)
		{
			int randIndex = Random.Range(0, pieceLibrary.Length - 1);
			if (pieceLibrary[randIndex] != null && !pieceLibrary[randIndex].activeInHierarchy)
			{
				GameObject newPiece = GameObject.Instantiate(pieceLibrary[randIndex].gameObject, 
				                                             new Vector3(0, 0, 0), Quaternion.identity) as GameObject;
				looking = false;
				return (newPiece);
			}
		}

		return (null);
	}

	public void GenerateWorld()
	{
		StartCoroutine(GenerateWorld_());
	}
	public IEnumerator GenerateWorld_()
	{
		GameObject rootPiece = CreatePiece();
		rootPiece.transform.Rotate(new Vector3(0, 0, Random.Range(0, 360)));

		List<slot> openSlots = new List<slot>();
		for (int i = 0;
		     i < rootPiece.GetComponent<WorldPieceController>().socketObjects.Length;
		     i++)
		{
			slot newSlot;
			newSlot.piece = rootPiece.GetComponent<WorldPieceController>().gameObject;
			newSlot.slotObject = rootPiece.GetComponent<WorldPieceController>().socketObjects[i];
			openSlots.Add(newSlot);
		}


		int levelSize = 20;
		for (int pieceNum = 1;
		     pieceNum < levelSize;
		     pieceNum++)
		{
			int loopCount = 0;
			bool looking = true;
			while (looking)
			{
				if (loopCount > 10)
				{
					looking = false;
					continue;
				}
				loopCount++;

				GameObject newPiece = CreatePiece();
				WorldPieceController newPieceCont = newPiece.GetComponent<WorldPieceController>();
				GameObject mover = new GameObject();

				int slotIndexFilling = Random.Range(0, openSlots.Count);
				slot slotFilling = openSlots[slotIndexFilling];
				GameObject slotUsing = newPieceCont.socketObjects[Random.Range(0, newPieceCont.socketObjects.Length - 1)];

				if ((slotFilling.slotObject.GetComponent<SocketController>().socketType != 
				    slotUsing.GetComponent<SocketController>().socketType))
				{
					DestroyImmediate(newPiece);
					DestroyImmediate(mover);
					continue;
				}

				mover.transform.position = slotUsing.transform.position;
				mover.transform.rotation = slotUsing.transform.rotation;
				newPiece.transform.parent = mover.transform;

				mover.transform.rotation = slotFilling.slotObject.transform.rotation;
				mover.transform.position = slotFilling.slotObject.transform.position;
				mover.transform.Rotate(0, 0, 180);
				newPiece.transform.parent = null;
				DestroyImmediate(mover);

				yield return new WaitForSeconds(0.03f);
				if (!newPieceCont.collided)
				{
					slotFilling.slotObject.GetComponent<SocketController>().isTaken = true;
					slotUsing.GetComponent<SocketController>().isTaken = true;
					looking = false;

					for (int i = 0;
					     i < newPiece.GetComponent<WorldPieceController>().socketObjects.Length;
					     i++)
					{
						slot newSlot;
						newSlot.piece = newPiece;
						newSlot.slotObject = newPiece.GetComponent<WorldPieceController>().socketObjects[i];
						if (newSlot.slotObject != slotFilling.slotObject)
						{
							openSlots.Add(newSlot);
						}
					}
				}
				else
				{
					DestroyImmediate(newPiece);
				}
			}
		}

		foreach (slot slotChecking in openSlots)
		{
			if (!slotChecking.slotObject.GetComponent<SocketController>().isTaken)
			{
				if (slotChecking.slotObject.GetComponent<SocketController>().socketType == "Single")
				{
					GameObject newObj = GameObject.Instantiate(singleEnd, slotChecking.slotObject.transform.position, 
					                                           Quaternion.identity) as GameObject;
					newObj.transform.rotation = slotChecking.slotObject.transform.rotation;
					newObj.transform.Rotate(0, 0, 90);
				}
				if (slotChecking.slotObject.GetComponent<SocketController>().socketType == "Double")
				{
					GameObject newObj = GameObject.Instantiate(doubleEnd, slotChecking.slotObject.transform.position, 
					                                           Quaternion.identity) as GameObject;
					newObj.transform.rotation = slotChecking.slotObject.transform.rotation;
					newObj.transform.Rotate(0, 0, 90);
				}
			}
		}
	}
}