namespace GroundTruther
{
  partial class GroundTruther
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
      this.LarcosLogo = new System.Windows.Forms.PictureBox ();
      this.ReBuildModel = new System.Windows.Forms.Button ();
      this.ClassifierLabel = new System.Windows.Forms.Label ();
      this.Classifier = new System.Windows.Forms.ComboBox ();
      this.panel1 = new System.Windows.Forms.Panel ();
      this.Count = new System.Windows.Forms.TextBox ();
      this.CountLabel = new System.Windows.Forms.Label ();
      this.TabDisplay = new System.Windows.Forms.TabControl ();
      this.ScannerFileViewer = new System.Windows.Forms.TabPage ();
      this.ThumbNailViewer = new System.Windows.Forms.TabPage ();
      this.ClassSelectionPanel = new System.Windows.Forms.FlowLayoutPanel ();
      ((System.ComponentModel.ISupportInitialize)(this.LarcosLogo)).BeginInit ();
      this.panel1.SuspendLayout ();
      this.TabDisplay.SuspendLayout ();
      this.SuspendLayout ();
      // 
      // LarcosLogo
      // 
      this.LarcosLogo.Image = global::GroundTruther.Properties.Resources.LarcosLogoSmall2;
      this.LarcosLogo.Location = new System.Drawing.Point (12, 12);
      this.LarcosLogo.Name = "LarcosLogo";
      this.LarcosLogo.Size = new System.Drawing.Size (156, 107);
      this.LarcosLogo.TabIndex = 0;
      this.LarcosLogo.TabStop = false;
      // 
      // ReBuildModel
      // 
      this.ReBuildModel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ReBuildModel.Location = new System.Drawing.Point (310, 17);
      this.ReBuildModel.Name = "ReBuildModel";
      this.ReBuildModel.Size = new System.Drawing.Size (75, 23);
      this.ReBuildModel.TabIndex = 70;
      this.ReBuildModel.Text = "Rebuild Model";
      this.ReBuildModel.UseVisualStyleBackColor = true;
      // 
      // ClassifierLabel
      // 
      this.ClassifierLabel.AutoSize = true;
      this.ClassifierLabel.Location = new System.Drawing.Point (3, 4);
      this.ClassifierLabel.Name = "ClassifierLabel";
      this.ClassifierLabel.Size = new System.Drawing.Size (48, 13);
      this.ClassifierLabel.TabIndex = 68;
      this.ClassifierLabel.Text = "Classifier";
      this.ClassifierLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // Classifier
      // 
      this.Classifier.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Classifier.FormattingEnabled = true;
      this.Classifier.Location = new System.Drawing.Point (6, 17);
      this.Classifier.Name = "Classifier";
      this.Classifier.Size = new System.Drawing.Size (300, 24);
      this.Classifier.TabIndex = 69;
      this.Classifier.Text = "None";
      // 
      // panel1
      // 
      this.panel1.Controls.Add (this.ClassifierLabel);
      this.panel1.Controls.Add (this.ReBuildModel);
      this.panel1.Controls.Add (this.Classifier);
      this.panel1.Location = new System.Drawing.Point (464, 12);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size (389, 46);
      this.panel1.TabIndex = 2;
      // 
      // Count
      // 
      this.Count.Font = new System.Drawing.Font ("Microsoft Sans Serif", 27.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Count.Location = new System.Drawing.Point (407, 693);
      this.Count.Name = "Count";
      this.Count.ReadOnly = true;
      this.Count.Size = new System.Drawing.Size (252, 49);
      this.Count.TabIndex = 3;
      // 
      // CountLabel
      // 
      this.CountLabel.AutoSize = true;
      this.CountLabel.Location = new System.Drawing.Point (404, 677);
      this.CountLabel.Name = "CountLabel";
      this.CountLabel.Size = new System.Drawing.Size (35, 13);
      this.CountLabel.TabIndex = 4;
      this.CountLabel.Text = "Count";
      // 
      // TabDisplay
      // 
      this.TabDisplay.Controls.Add (this.ScannerFileViewer);
      this.TabDisplay.Controls.Add (this.ThumbNailViewer);
      this.TabDisplay.Location = new System.Drawing.Point (192, 137);
      this.TabDisplay.Name = "TabDisplay";
      this.TabDisplay.SelectedIndex = 0;
      this.TabDisplay.Size = new System.Drawing.Size (744, 516);
      this.TabDisplay.TabIndex = 5;
      // 
      // ScannerFileViewer
      // 
      this.ScannerFileViewer.Location = new System.Drawing.Point (4, 22);
      this.ScannerFileViewer.Name = "ScannerFileViewer";
      this.ScannerFileViewer.Padding = new System.Windows.Forms.Padding (3);
      this.ScannerFileViewer.Size = new System.Drawing.Size (736, 490);
      this.ScannerFileViewer.TabIndex = 0;
      this.ScannerFileViewer.Text = "Scanner File View";
      this.ScannerFileViewer.UseVisualStyleBackColor = true;
      // 
      // ThumbNailViewer
      // 
      this.ThumbNailViewer.Location = new System.Drawing.Point (4, 22);
      this.ThumbNailViewer.Name = "ThumbNailViewer";
      this.ThumbNailViewer.Padding = new System.Windows.Forms.Padding (3);
      this.ThumbNailViewer.Size = new System.Drawing.Size (736, 490);
      this.ThumbNailViewer.TabIndex = 1;
      this.ThumbNailViewer.Text = "ThumbNailView";
      this.ThumbNailViewer.UseVisualStyleBackColor = true;
      // 
      // ClassSelectionPanel
      // 
      this.ClassSelectionPanel.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
      this.ClassSelectionPanel.Location = new System.Drawing.Point (12, 160);
      this.ClassSelectionPanel.Name = "ClassSelectionPanel";
      this.ClassSelectionPanel.Size = new System.Drawing.Size (174, 494);
      this.ClassSelectionPanel.TabIndex = 6;
      // 
      // GroundTruther
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (948, 754);
      this.Controls.Add (this.ClassSelectionPanel);
      this.Controls.Add (this.TabDisplay);
      this.Controls.Add (this.CountLabel);
      this.Controls.Add (this.Count);
      this.Controls.Add (this.panel1);
      this.Controls.Add (this.LarcosLogo);
      this.Name = "GroundTruther";
      this.Text = "Ground Truther";
      ((System.ComponentModel.ISupportInitialize)(this.LarcosLogo)).EndInit ();
      this.panel1.ResumeLayout (false);
      this.panel1.PerformLayout ();
      this.TabDisplay.ResumeLayout (false);
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.PictureBox LarcosLogo;
    private System.Windows.Forms.Button ReBuildModel;
    private System.Windows.Forms.Label ClassifierLabel;
    private System.Windows.Forms.ComboBox Classifier;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.TextBox Count;
    private System.Windows.Forms.Label CountLabel;
    private System.Windows.Forms.TabControl TabDisplay;
    private System.Windows.Forms.TabPage ScannerFileViewer;
    private System.Windows.Forms.TabPage ThumbNailViewer;
    private System.Windows.Forms.FlowLayoutPanel ClassSelectionPanel;
  }
}

