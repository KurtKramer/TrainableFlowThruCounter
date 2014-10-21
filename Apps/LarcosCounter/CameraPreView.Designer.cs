namespace LarcosCounter
{
  partial class CameraPreView
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
      this.components = new System.ComponentModel.Container ();
      this.CameraFrame = new System.Windows.Forms.PictureBox ();
      this.RefreshTimer = new System.Windows.Forms.Timer (this.components);
      this.Count = new System.Windows.Forms.TextBox ();
      this.ResetButton = new System.Windows.Forms.Button ();
      this.CameraFrame1 = new System.Windows.Forms.PictureBox ();
      this.CameraFrame2 = new System.Windows.Forms.PictureBox ();
      this.CameraFrame3 = new System.Windows.Forms.PictureBox ();
      this.CameraFrame4 = new System.Windows.Forms.PictureBox ();
      this.CameraFrame5 = new System.Windows.Forms.PictureBox ();
      this.CameraFrame0label = new System.Windows.Forms.TextBox ();
      this.CameraFrame1label = new System.Windows.Forms.TextBox ();
      this.CameraFrame2label = new System.Windows.Forms.TextBox ();
      this.CameraFrame3label = new System.Windows.Forms.TextBox ();
      this.CameraFrame4label = new System.Windows.Forms.TextBox ();
      this.CameraFrame5label = new System.Windows.Forms.TextBox ();
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame1)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame2)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame3)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame4)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame5)).BeginInit ();
      this.SuspendLayout ();
      // 
      // CameraFrame
      // 
      this.CameraFrame.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.CameraFrame.Location = new System.Drawing.Point (10, 97);
      this.CameraFrame.Name = "CameraFrame";
      this.CameraFrame.Size = new System.Drawing.Size (188, 179);
      this.CameraFrame.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
      this.CameraFrame.TabIndex = 0;
      this.CameraFrame.TabStop = false;
      // 
      // RefreshTimer
      // 
      this.RefreshTimer.Enabled = true;
      this.RefreshTimer.Interval = 200;
      this.RefreshTimer.Tick += new System.EventHandler (this.RefreshTimer_Tick);
      // 
      // Count
      // 
      this.Count.Font = new System.Drawing.Font ("Microsoft Sans Serif", 48F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Count.Location = new System.Drawing.Point (169, 11);
      this.Count.Name = "Count";
      this.Count.ReadOnly = true;
      this.Count.Size = new System.Drawing.Size (269, 80);
      this.Count.TabIndex = 1;
      this.Count.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // ResetButton
      // 
      this.ResetButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ResetButton.Location = new System.Drawing.Point (265, 523);
      this.ResetButton.Name = "ResetButton";
      this.ResetButton.Size = new System.Drawing.Size (114, 47);
      this.ResetButton.TabIndex = 2;
      this.ResetButton.Text = "Reset";
      this.ResetButton.UseVisualStyleBackColor = true;
      this.ResetButton.Click += new System.EventHandler (this.ResetButton_Click);
      // 
      // CameraFrame1
      // 
      this.CameraFrame1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.CameraFrame1.Location = new System.Drawing.Point (217, 97);
      this.CameraFrame1.Name = "CameraFrame1";
      this.CameraFrame1.Size = new System.Drawing.Size (188, 179);
      this.CameraFrame1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
      this.CameraFrame1.TabIndex = 3;
      this.CameraFrame1.TabStop = false;
      // 
      // CameraFrame2
      // 
      this.CameraFrame2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.CameraFrame2.Location = new System.Drawing.Point (422, 97);
      this.CameraFrame2.Name = "CameraFrame2";
      this.CameraFrame2.Size = new System.Drawing.Size (188, 179);
      this.CameraFrame2.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
      this.CameraFrame2.TabIndex = 4;
      this.CameraFrame2.TabStop = false;
      // 
      // CameraFrame3
      // 
      this.CameraFrame3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.CameraFrame3.Location = new System.Drawing.Point (10, 306);
      this.CameraFrame3.Name = "CameraFrame3";
      this.CameraFrame3.Size = new System.Drawing.Size (188, 179);
      this.CameraFrame3.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
      this.CameraFrame3.TabIndex = 5;
      this.CameraFrame3.TabStop = false;
      // 
      // CameraFrame4
      // 
      this.CameraFrame4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.CameraFrame4.Location = new System.Drawing.Point (217, 306);
      this.CameraFrame4.Name = "CameraFrame4";
      this.CameraFrame4.Size = new System.Drawing.Size (188, 179);
      this.CameraFrame4.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
      this.CameraFrame4.TabIndex = 6;
      this.CameraFrame4.TabStop = false;
      // 
      // CameraFrame5
      // 
      this.CameraFrame5.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.CameraFrame5.Location = new System.Drawing.Point (422, 306);
      this.CameraFrame5.Name = "CameraFrame5";
      this.CameraFrame5.Size = new System.Drawing.Size (188, 179);
      this.CameraFrame5.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
      this.CameraFrame5.TabIndex = 7;
      this.CameraFrame5.TabStop = false;
      // 
      // CameraFrame0label
      // 
      this.CameraFrame0label.Font = new System.Drawing.Font ("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CameraFrame0label.Location = new System.Drawing.Point (10, 278);
      this.CameraFrame0label.Name = "CameraFrame0label";
      this.CameraFrame0label.ReadOnly = true;
      this.CameraFrame0label.Size = new System.Drawing.Size (188, 24);
      this.CameraFrame0label.TabIndex = 8;
      this.CameraFrame0label.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.CameraFrame0label.TextChanged += new System.EventHandler (this.CameraFrame0label_TextChanged);
      // 
      // CameraFrame1label
      // 
      this.CameraFrame1label.Font = new System.Drawing.Font ("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CameraFrame1label.Location = new System.Drawing.Point (217, 278);
      this.CameraFrame1label.Name = "CameraFrame1label";
      this.CameraFrame1label.ReadOnly = true;
      this.CameraFrame1label.Size = new System.Drawing.Size (188, 24);
      this.CameraFrame1label.TabIndex = 9;
      this.CameraFrame1label.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.CameraFrame1label.TextChanged += new System.EventHandler (this.CameraFrame1label_TextChanged);
      // 
      // CameraFrame2label
      // 
      this.CameraFrame2label.Font = new System.Drawing.Font ("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CameraFrame2label.Location = new System.Drawing.Point (422, 278);
      this.CameraFrame2label.Name = "CameraFrame2label";
      this.CameraFrame2label.ReadOnly = true;
      this.CameraFrame2label.Size = new System.Drawing.Size (188, 24);
      this.CameraFrame2label.TabIndex = 10;
      this.CameraFrame2label.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // CameraFrame3label
      // 
      this.CameraFrame3label.Font = new System.Drawing.Font ("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CameraFrame3label.Location = new System.Drawing.Point (10, 488);
      this.CameraFrame3label.Name = "CameraFrame3label";
      this.CameraFrame3label.ReadOnly = true;
      this.CameraFrame3label.Size = new System.Drawing.Size (188, 24);
      this.CameraFrame3label.TabIndex = 11;
      this.CameraFrame3label.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // CameraFrame4label
      // 
      this.CameraFrame4label.Font = new System.Drawing.Font ("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CameraFrame4label.Location = new System.Drawing.Point (217, 488);
      this.CameraFrame4label.Name = "CameraFrame4label";
      this.CameraFrame4label.ReadOnly = true;
      this.CameraFrame4label.Size = new System.Drawing.Size (188, 24);
      this.CameraFrame4label.TabIndex = 12;
      this.CameraFrame4label.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // CameraFrame5label
      // 
      this.CameraFrame5label.Font = new System.Drawing.Font ("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CameraFrame5label.Location = new System.Drawing.Point (422, 488);
      this.CameraFrame5label.Name = "CameraFrame5label";
      this.CameraFrame5label.ReadOnly = true;
      this.CameraFrame5label.Size = new System.Drawing.Size (188, 24);
      this.CameraFrame5label.TabIndex = 13;
      this.CameraFrame5label.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // CameraPreView
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (627, 580);
      this.Controls.Add (this.CameraFrame5label);
      this.Controls.Add (this.CameraFrame4label);
      this.Controls.Add (this.CameraFrame3label);
      this.Controls.Add (this.CameraFrame2label);
      this.Controls.Add (this.CameraFrame1label);
      this.Controls.Add (this.CameraFrame0label);
      this.Controls.Add (this.CameraFrame5);
      this.Controls.Add (this.CameraFrame4);
      this.Controls.Add (this.CameraFrame3);
      this.Controls.Add (this.CameraFrame2);
      this.Controls.Add (this.CameraFrame1);
      this.Controls.Add (this.ResetButton);
      this.Controls.Add (this.Count);
      this.Controls.Add (this.CameraFrame);
      this.MinimumSize = new System.Drawing.Size (304, 332);
      this.Name = "CameraPreView";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Sampling";
      this.Load += new System.EventHandler (this.CameraPreView_Load);
      this.SizeChanged += new System.EventHandler (this.CameraPreView_SizeChanged);
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame1)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame2)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame3)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame4)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.CameraFrame5)).EndInit ();
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.PictureBox CameraFrame;
    private System.Windows.Forms.Timer RefreshTimer;
    private System.Windows.Forms.TextBox Count;
    private System.Windows.Forms.Button ResetButton;
    private System.Windows.Forms.PictureBox CameraFrame1;
    private System.Windows.Forms.PictureBox CameraFrame2;
    private System.Windows.Forms.PictureBox CameraFrame3;
    private System.Windows.Forms.PictureBox CameraFrame4;
    private System.Windows.Forms.PictureBox CameraFrame5;
    private System.Windows.Forms.TextBox CameraFrame0label;
    private System.Windows.Forms.TextBox CameraFrame1label;
    private System.Windows.Forms.TextBox CameraFrame2label;
    private System.Windows.Forms.TextBox CameraFrame3label;
    private System.Windows.Forms.TextBox CameraFrame4label;
    private System.Windows.Forms.TextBox CameraFrame5label;
  }
}