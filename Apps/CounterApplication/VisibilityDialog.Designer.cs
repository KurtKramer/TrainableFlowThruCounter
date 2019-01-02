namespace CounterApplication
{
  partial class VisibilityDialog
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
      this.cameraLine = new CounterControls.CameraLine ();
      this.SuspendLayout ();
      // 
      // cameraLine
      // 
      this.cameraLine.Font = new System.Drawing.Font ("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.cameraLine.Location = new System.Drawing.Point (12, 12);
      this.cameraLine.Name = "cameraLine";
      this.cameraLine.RequestedSensitivityMode = "Low";
      this.cameraLine.Size = new System.Drawing.Size (848, 450);
      this.cameraLine.TabIndex = 0;
      // 
      // VisibilityDialog
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (872, 474);
      this.Controls.Add (this.cameraLine);
      this.MinimumSize = new System.Drawing.Size (888, 512);
      this.Name = "VisibilityDialog";
      this.Text = "Visibility Dialog";
      this.Load += new System.EventHandler (this.VisibilityDialog_Load);
      this.SizeChanged += new System.EventHandler (this.VisibilityDialog_SizeChanged);
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler (this.VisibilityDialog_FormClosing);
      this.ResumeLayout (false);

    }

    #endregion

    private CounterControls.CameraLine cameraLine;
  }
}