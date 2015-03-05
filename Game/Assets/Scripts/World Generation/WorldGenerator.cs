using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class WorldGenerator : MonoBehaviour
{
	public static WorldGenerator instance;

	public GameObject[] worldPieceLibrary;
	public GameObject[] enemyPieceLibrary;
	public GameObject singleEnd;
	public GameObject doubleEnd;
	public GameObject ladder;

	public bool isGenerating;

	public GameObject miniMapCamera;

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

	public GameObject CreatePiece()
	{
		bool looking = true;
		while (looking)
		{
			int randIndex = Random.Range(0, worldPieceLibrary.Length - 1);
			if (worldPieceLibrary[randIndex] != null && !worldPieceLibrary[randIndex].activeInHierarchy)
			{
				GameObject newPiece = GameObject.Instantiate(worldPieceLibrary[randIndex].gameObject, 
				                                             new Vector3(0, 0, 0), Quaternion.identity) as GameObject;
				looking = false;
				return (newPiece);
			}
		}

		return (null);
	}

	public void GenerateWorld()
	{
		isGenerating = true;
		StartCoroutine(GenerateWorld_());
	}
	public IEnumerator GenerateWorld_()
	{
		List<WorldPieceController> allWorldPieces = new List<WorldPieceController>();
		GameObject rootPiece = CreatePiece();
		allWorldPieces.Add(rootPiece.GetComponent<WorldPieceController>());
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


		int levelSize = 10;
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
					allWorldPieces.Add(newPieceCont);

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


		List<Vector3> openEnemyPositions = new List<Vector3>();
		for (int pieceIndex = 0;
		     pieceIndex < allWorldPieces.Count;
		     pieceIndex++)
		{
			for (int enemyIndex = 0;
			     enemyIndex < allWorldPieces[pieceIndex].enemySockets.Length;
			     enemyIndex++)
			{
				openEnemyPositions.Add(allWorldPieces[pieceIndex].enemySockets[enemyIndex].transform.position);
			}
		}

		int ladderIndex = Random.Range(0, openEnemyPositions.Count);
		GameObject.Instantiate(ladder, openEnemyPositions[ladderIndex], Quaternion.identity);

		foreach(Vector3 enemyPosition in openEnemyPositions)
		{
			if (Vector3.Distance(enemyPosition, PlayerController.instance.transform.position) > 5)
			{
				int fill = Random.Range(0, 100);
				if (fill < 80)
				{
					int pieceIndex = Random.Range(0, enemyPieceLibrary.Length);
					if (enemyPieceLibrary[pieceIndex] != null)
					{
						GameObject.Instantiate(enemyPieceLibrary[pieceIndex], enemyPosition, Quaternion.identity);
					}
				}
			}
		}


		Vector3 centerPos = new Vector3(0, 0, 0);
		for (int pieceIndex = 0;
		     pieceIndex < allWorldPieces.Count;
		     pieceIndex++)
		{
			GameObject mapPiece = GameObject.Instantiate(allWorldPieces[pieceIndex].gameObject, 
			                                             allWorldPieces[pieceIndex].gameObject.transform.position, 
			                                             allWorldPieces[pieceIndex].gameObject.transform.rotation) as GameObject;
			Vector3 newPos = new Vector3(mapPiece.transform.position.x + 2000, mapPiece.transform.position.y + 2000, mapPiece.transform.position.z + 2000);
			mapPiece.transform.position = newPos;
			allWorldPieces[pieceIndex].miniMapPiece = mapPiece;
			mapPiece.SetActive(false);

			centerPos += newPos;
		}
		centerPos = centerPos / allWorldPieces.Count;
		centerPos += new Vector3(0, 0, -250);
		miniMapCamera.transform.position = centerPos;

		isGenerating = false;
	}
}