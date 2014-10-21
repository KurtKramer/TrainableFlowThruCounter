namespace LarcosFileViewer
{
  partial class GoToScanLinePrompt
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose (bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose ();
      }
      base.Dispose (disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent ()
    {
      this.ScanLine = new System.Windows.Forms.TextBox ();
      this.ScanLineLabel = new System.Windows.Forms.Label ();
      this.GotoButton = new System.Windows.Forms.Button ();
      this.CancelButton = new System.Windows.Forms.Button ();
      this.SuspendLayout ();
      // 
      // ScanLine
      // 
      this.ScanLine.Location = new System.Drawing.Point (15, 25);
      this.ScanLine.Name = "ScanLine";
      this.ScanLine.Size = new System.Drawing.Size (130, 20);
      this.ScanLine.TabIndex = 1;
      // 
      // ScanLineLabel
      // 
      this.ScanLineLabel.AutoSize = true;
      this.ScanLineLabel.Location = new System.Drawing.Point (12, 9);
      this.ScanLineLabel.Name = "ScanLineLabel";
      this.ScanLineLabel.Size = new System.Drawing.Size (55, 13);
      this.ScanLineLabel.TabIndex = 2;
      this.ScanLineLabel.Text = "Scan Line";
      // 
      // GotoButton
      // 
      this.GotoButton.Location = new System.Drawing.Point (15, 67);
      this.GotoButton.Name = "GotoButton";
      this.GotoButton.Size = new System.Drawing.Size (75, 23);
      this.GotoButton.TabIndex = 3;
      this.GotoButton.Text = "Goto";
      this.GotoButton.UseVisualStyleBackColor = true;
      this.GotoButton.Click += new System.EventHandler (this.GotoButton_Click);
      // 
      // CancelButton
      // 
      this.CancelButton.Location = new System.Drawing.Point (125, 67);
      this.CancelButton.Name = "CancelButton";
      this.CancelButton.Size = new System.Drawing.Size (75, 23);
      this.CancelButton.TabIndex = 4;
      this.CancelButton.Text = "Cancel";
      this.CancelButton.UseVisualStyleBackColor = true;
      this.CancelButton.Click += new System.EventHandler (this.CancelButton_Click);
      // 
      // GoToScanLinePrompt
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (218, 106);
      this.Controls.Add (this.CancelButton);
      this.Controls.Add (this.GotoButton);
      this.Controls.Add (this.ScanLineLabel);
      this.Controls.Add (this.ScanLine);
      this.Name = "GoToScanLinePrompt";
      this.Text = "GoToScanLinePrompt";
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.TextBox ScanLine;
    private System.Windows.Forms.Label ScanLineLabel;
    private System.Windows.Forms.Button GotoButton;
    private System.Windows.Forms.Button CancelButton;
  }
}