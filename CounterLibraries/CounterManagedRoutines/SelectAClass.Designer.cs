namespace CounterManagedRoutines
{
  partial class SelectAClass
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.ImageClassChoices = new System.Windows.Forms.TreeView ();
      this.CancelButon = new System.Windows.Forms.Button ();
      this.SelectClassButton = new System.Windows.Forms.Button ();
      this.SuspendLayout ();
      // 
      // ImageClassChoices
      // 
      this.ImageClassChoices.Location = new System.Drawing.Point (13, 14);
      this.ImageClassChoices.Name = "ImageClassChoices";
      this.ImageClassChoices.Size = new System.Drawing.Size (363, 468);
      this.ImageClassChoices.TabIndex = 0;
      this.ImageClassChoices.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler (this.ImageClassChoices_NodeMouseDoubleClick);
      // 
      // CancelButon
      // 
      this.CancelButon.Location = new System.Drawing.Point (301, 494);
      this.CancelButon.Name = "CancelButon";
      this.CancelButon.Size = new System.Drawing.Size (75, 23);
      this.CancelButon.TabIndex = 1;
      this.CancelButon.Text = "Cancel";
      this.CancelButon.UseVisualStyleBackColor = true;
      this.CancelButon.Click += new System.EventHandler (this.CancelButon_Click);
      // 
      // SelectClassButton
      // 
      this.SelectClassButton.Location = new System.Drawing.Point (13, 494);
      this.SelectClassButton.Name = "SelectClassButton";
      this.SelectClassButton.Size = new System.Drawing.Size (75, 23);
      this.SelectClassButton.TabIndex = 2;
      this.SelectClassButton.Text = "Select Class";
      this.SelectClassButton.UseVisualStyleBackColor = true;
      this.SelectClassButton.Click += new System.EventHandler (this.SelectClassButton_Click);
      // 
      // SelectAClass
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (388, 529);
      this.Controls.Add (this.SelectClassButton);
      this.Controls.Add (this.CancelButon);
      this.Controls.Add (this.ImageClassChoices);
      this.Name = "SelectAClass";
      this.Text = "SelectAClass";
      this.Load += new System.EventHandler (this.SelectAClass_Load);
      this.Resize += new System.EventHandler (this.SelectAClass_Resize);
      this.ResumeLayout (false);

    }

    #endregion

    private System.Windows.Forms.TreeView ImageClassChoices;
    private System.Windows.Forms.Button CancelButon;
    private System.Windows.Forms.Button SelectClassButton;
  }
}