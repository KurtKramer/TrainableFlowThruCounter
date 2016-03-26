namespace LarcosManagedRoutines
{
  partial class AddAClass
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
      this.components = new System.ComponentModel.Container ();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager (typeof (AddAClass));
      this.ClassNameLabel = new System.Windows.Forms.Label ();
      this.CountLabel = new System.Windows.Forms.Label ();
      this.ClassNameField = new System.Windows.Forms.TextBox ();
      this.CountField = new System.Windows.Forms.NumericUpDown ();
      this.IntroField = new System.Windows.Forms.TextBox ();
      this.AddNewClassButton = new System.Windows.Forms.Button ();
      this.CancelButton = new System.Windows.Forms.Button ();
      this.errorProvider1 = new System.Windows.Forms.ErrorProvider (this.components);
      this.TraihningLibraryNameField = new System.Windows.Forms.TextBox ();
      ((System.ComponentModel.ISupportInitialize)(this.CountField)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).BeginInit ();
      this.SuspendLayout ();
      // 
      // ClassNameLabel
      // 
      this.ClassNameLabel.AutoSize = true;
      this.ClassNameLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ClassNameLabel.Location = new System.Drawing.Point (14, 275);
      this.ClassNameLabel.Name = "ClassNameLabel";
      this.ClassNameLabel.Size = new System.Drawing.Size (82, 16);
      this.ClassNameLabel.TabIndex = 0;
      this.ClassNameLabel.Text = "Class Name";
      // 
      // CountLabel
      // 
      this.CountLabel.AutoSize = true;
      this.CountLabel.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CountLabel.Location = new System.Drawing.Point (14, 317);
      this.CountLabel.Name = "CountLabel";
      this.CountLabel.Size = new System.Drawing.Size (42, 16);
      this.CountLabel.TabIndex = 1;
      this.CountLabel.Text = "Count";
      // 
      // ClassNameField
      // 
      this.ClassNameField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ClassNameField.Location = new System.Drawing.Point (102, 269);
      this.ClassNameField.Name = "ClassNameField";
      this.ClassNameField.Size = new System.Drawing.Size (270, 26);
      this.ClassNameField.TabIndex = 100;
      // 
      // CountField
      // 
      this.CountField.DecimalPlaces = 1;
      this.CountField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CountField.Location = new System.Drawing.Point (102, 312);
      this.CountField.Name = "CountField";
      this.CountField.Size = new System.Drawing.Size (82, 26);
      this.CountField.TabIndex = 200;
      this.CountField.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // IntroField
      // 
      this.IntroField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.IntroField.Location = new System.Drawing.Point (12, 52);
      this.IntroField.Multiline = true;
      this.IntroField.Name = "IntroField";
      this.IntroField.ReadOnly = true;
      this.IntroField.Size = new System.Drawing.Size (462, 193);
      this.IntroField.TabIndex = 4;
      this.IntroField.TabStop = false;
      this.IntroField.Text = resources.GetString ("IntroField.Text");
      // 
      // AddNewClassButton
      // 
      this.AddNewClassButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.AddNewClassButton.Location = new System.Drawing.Point (17, 380);
      this.AddNewClassButton.Name = "AddNewClassButton";
      this.AddNewClassButton.Size = new System.Drawing.Size (145, 32);
      this.AddNewClassButton.TabIndex = 300;
      this.AddNewClassButton.Text = "Add New Class";
      this.AddNewClassButton.UseVisualStyleBackColor = true;
      this.AddNewClassButton.Click += new System.EventHandler (this.AddNewClassButton_Click);
      // 
      // CancelButton
      // 
      this.CancelButton.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CancelButton.Location = new System.Drawing.Point (394, 380);
      this.CancelButton.Name = "CancelButton";
      this.CancelButton.Size = new System.Drawing.Size (80, 32);
      this.CancelButton.TabIndex = 400;
      this.CancelButton.Text = "Cancel";
      this.CancelButton.UseVisualStyleBackColor = true;
      this.CancelButton.Click += new System.EventHandler (this.CancelButton_Click);
      // 
      // errorProvider1
      // 
      this.errorProvider1.ContainerControl = this;
      // 
      // TraihningLibraryNameField
      // 
      this.TraihningLibraryNameField.Font = new System.Drawing.Font ("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TraihningLibraryNameField.Location = new System.Drawing.Point (12, 12);
      this.TraihningLibraryNameField.Name = "TraihningLibraryNameField";
      this.TraihningLibraryNameField.ReadOnly = true;
      this.TraihningLibraryNameField.Size = new System.Drawing.Size (462, 26);
      this.TraihningLibraryNameField.TabIndex = 7;
      this.TraihningLibraryNameField.TabStop = false;
      this.TraihningLibraryNameField.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // AddAClass
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (486, 427);
      this.Controls.Add (this.TraihningLibraryNameField);
      this.Controls.Add (this.CancelButton);
      this.Controls.Add (this.AddNewClassButton);
      this.Controls.Add (this.IntroField);
      this.Controls.Add (this.CountField);
      this.Controls.Add (this.ClassNameField);
      this.Controls.Add (this.CountLabel);
      this.Controls.Add (this.ClassNameLabel);
      this.Name = "AddAClass";
      this.Text = "Add A Class";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler (this.AddAClass_FormClosing);
      ((System.ComponentModel.ISupportInitialize)(this.CountField)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).EndInit ();
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.Label ClassNameLabel;
    private System.Windows.Forms.Label CountLabel;
    private System.Windows.Forms.TextBox ClassNameField;
    private System.Windows.Forms.NumericUpDown CountField;
    private System.Windows.Forms.TextBox IntroField;
    private System.Windows.Forms.Button AddNewClassButton;
    private System.Windows.Forms.Button CancelButton;
    private System.Windows.Forms.ErrorProvider errorProvider1;
    private System.Windows.Forms.TextBox TraihningLibraryNameField;
  }
}