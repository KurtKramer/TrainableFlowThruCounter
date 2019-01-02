namespace CounterControls
{
  partial class ScannerFileViewerPanel
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

    #region Component Designer generated code

    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent ()
    {
      this.panel = new System.Windows.Forms.Panel ();
      this.IncludeExcludePanel = new System.Windows.Forms.Panel ();
      this.SuspendLayout ();
      // 
      // panel
      // 
      this.panel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.panel.Location = new System.Drawing.Point (1, 0);
      this.panel.Name = "panel";
      this.panel.Size = new System.Drawing.Size (798, 585);
      this.panel.TabIndex = 0;
      this.panel.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler (this.panel_MouseDoubleClick);
      // 
      // IncludeExcludePanel
      // 
      this.IncludeExcludePanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.IncludeExcludePanel.Location = new System.Drawing.Point (1, 587);
      this.IncludeExcludePanel.Name = "IncludeExcludePanel";
      this.IncludeExcludePanel.Size = new System.Drawing.Size (798, 11);
      this.IncludeExcludePanel.TabIndex = 1;
      // 
      // ScannerFileViewerPanel
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add (this.IncludeExcludePanel);
      this.Controls.Add (this.panel);
      this.Name = "ScannerFileViewerPanel";
      this.Size = new System.Drawing.Size (800, 600);
      this.Load += new System.EventHandler (this.ScannerFileViewerPanel_Load);
      this.VisibleChanged += new System.EventHandler (this.ScannerFileViewerPanel_VisibleChanged);
      this.SizeChanged += new System.EventHandler (this.ScannerFileViewerPanel_SizeChanged);
      this.ResumeLayout (false);

    }

    #endregion

    private System.Windows.Forms.Panel panel;
    private System.Windows.Forms.Panel IncludeExcludePanel;
  }
}
