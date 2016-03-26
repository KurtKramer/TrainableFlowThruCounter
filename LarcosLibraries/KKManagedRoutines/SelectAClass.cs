using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using LarcosCounterManaged;

namespace KKManagedRoutines
{
  public partial class SelectAClass : Form
  {
    private UmiClass  selectedClass = null;

    public  UmiClass  SelectedClass  
    {
      get {return selectedClass;}
      set 
      {
        selectedClass = value;
        SetSelectTreeNode (ImageClassChoices.Nodes);
      }
    }


    public  bool  SelectionMade   {get {return selectionMade;}}


    private Font  activeClassFont = null;

    private  int  lastWidth = 0;
    private  int  lastHeight = 0;

    private  String  rootDir;

    private  bool  selectionMade = false;

    private  UmiClassList  activeClasses = null;

    private  TreeNode  selectedNode = null;





    public SelectAClass (UmiClassList  classes,
                         String        _rootDir,
                         UmiClassList  _activeClasses
                        )
    {
      activeClasses = _activeClasses;
      rootDir = UmiOSservices.AddSlash (_rootDir);
      InitializeComponent();

      activeClassFont = new Font (ImageClassChoices.Font.FontFamily,ImageClassChoices.Font.SizeInPoints + 1, FontStyle.Bold);

      selectedNode = null;

      UmiClass  root = classes.RootNode;
      if  (root == null)
        root = UmiClassList.GetUniqueClass ("AllClasses", "");

      foreach  (UmiClass c in classes)
      {
        if  (c.Parent == null)
          root.AddAChild (c);
      }

      bool activeClass = false;
      AddSubTreeToClassChoser (root, ImageClassChoices.Nodes, ref activeClass);

      foreach  (UmiClass c in classes)
      {
        if  (!c.IsAncestor (root))
        {
          AddSubTreeToClassChoser (c, ImageClassChoices.Nodes, ref activeClass);
        }
      }

      if  (selectedNode != null)
        ImageClassChoices.SelectedNode = selectedNode;

      //foreach  (UmiClass pc in root.Children)
      //  AddSubTreeToClassChoser (pc, ImageClassChoices.Nodes, ref activeClass);
    }


    private  void  SetSelectTreeNode (TreeNodeCollection nodes)
    {
      if  ((nodes == null)  ||  (selectedClass == null))
        return;

      selectedNode = null;

      foreach (TreeNode tn in nodes)
      {
        if  (tn.Text == selectedClass.Name)
        {
          selectedNode = tn;
          ImageClassChoices.SelectedNode = selectedNode;
          return;
        }
        else if  (tn.Nodes != null)
        {
          SetSelectTreeNode (tn.Nodes);
          if  (selectedNode != null)
            return;
        }
      }
    }  /* SetSelectTreeNode */



        
    private  void  AddSubTreeToClassChoser (UmiClass           pc, 
                                            TreeNodeCollection nodes,
                                            ref bool           activeClass
                                           )
    {
      bool  iAmActive = false;
      if (activeClasses != null)
        iAmActive = (activeClasses.LookUpIndex (pc) >= 0);
      bool  oneOfMyChildrenAreActive = false;

      TreeNode  tn = new TreeNode (pc.Name);
      nodes.Add (tn);
      if  (pc == selectedClass)
        selectedNode = tn;

      foreach  (UmiClass  child  in  pc.Children)
      {
        bool  childActive = false;
        AddSubTreeToClassChoser (child, tn.Nodes, ref childActive);
        if  (childActive)
          oneOfMyChildrenAreActive = true;
      }

      if  (iAmActive  ||  oneOfMyChildrenAreActive)
      {
        activeClass = true;
        if  (iAmActive)
        {
          tn.ForeColor = Color.DarkGreen;
          tn.NodeFont = activeClassFont;
        }
        else if  (oneOfMyChildrenAreActive)
          tn.ForeColor = Color.Green;
      }
      else
      {
        tn.ForeColor = Color.Gray;
      }
    }



    private void CancelButon_Click(object sender, EventArgs e)
    {
      selectedClass = null;
      Close ();
      return;
    }


    private void ImageClassChoices_NodeMouseDoubleClick (object sender, TreeNodeMouseClickEventArgs e)
    {
      if  (e.Button == MouseButtons.Left)
      {
        selectedClass = UmiClassList.GetUniqueClass (e.Node.Text, "");
        selectionMade = true;
        Close ();
        return;
      }
      else if  (e.Button == MouseButtons.Right)
      {
        if  (String.IsNullOrEmpty (rootDir))
          return;

        String  fullDirPath = rootDir + e.Node.Text;

        DisplayScsImages  dpi = new DisplayScsImages (fullDirPath);
        dpi.ShowDialog ();
        dpi.Dispose ();
        dpi = null;
      }
    }


    private void SelectAClass_Resize(object sender, EventArgs e)
    {
      int  deltaHeight = Height - lastHeight;
      int  deltaWidth  = Width  - lastWidth;

      CancelButon.Top += deltaHeight;
      CancelButon.Left += deltaWidth;
      ImageClassChoices.Width += deltaWidth;
      ImageClassChoices.Height += deltaHeight;

      SelectClassButton.Top += deltaHeight;

      lastHeight = Height;
      lastWidth  = Width;
    }


    private void SelectClassButton_Click (object sender, EventArgs e)
    {
      if  (ImageClassChoices.SelectedNode != null)
      {
        selectedClass = UmiClassList.GetUniqueClass (ImageClassChoices.SelectedNode.Text, "");
        selectionMade = true;
        Close ();
        return;
      }
    }


    private void SelectAClass_Load (object sender, EventArgs e)
    {
      lastHeight = Height;
      lastWidth  = Width;
    }
  }
}