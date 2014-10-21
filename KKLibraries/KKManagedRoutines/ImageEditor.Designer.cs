namespace KKManagedRoutines
{
  partial class ImageEditor
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void  Dispose (bool disposing)
    {
      if  (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
      
      if  (image != null)
      {
        image.Dispose();
        image = null;
      }
      raster = null;
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.pictureBox1 = new System.Windows.Forms.PictureBox ();
      this.ImagePanel = new System.Windows.Forms.Panel ();
      this.pred1ClassName = new System.Windows.Forms.TextBox ();
      this.pred1Prob = new System.Windows.Forms.TextBox ();
      this.TrainLib1Name = new System.Windows.Forms.TextBox ();
      this.Colorize = new System.Windows.Forms.CheckBox ();
      this.ClossingButton = new System.Windows.Forms.Button ();
      this.OpeningButton = new System.Windows.Forms.Button ();
      this.ErosionButton = new System.Windows.Forms.Button ();
      this.DialationButton = new System.Windows.Forms.Button ();
      this.EdgeButton = new System.Windows.Forms.Button ();
      this.FillHolesButton = new System.Windows.Forms.Button ();
      this.SmoothingButton = new System.Windows.Forms.Button ();
      this.MediumButton = new System.Windows.Forms.Button ();
      this.StructuredOperations = new System.Windows.Forms.Panel ();
      this.MaskExcludeButton = new System.Windows.Forms.Button ();
      this.MaskTypesComboBox = new System.Windows.Forms.ComboBox ();
      this.ThinningButton = new System.Windows.Forms.Button ();
      this.UndoButton = new System.Windows.Forms.Button ();
      this.OperationsList = new System.Windows.Forms.ListBox ();
      this.ConnectedComponentButton = new System.Windows.Forms.Button ();
      this.BinarizeButton = new System.Windows.Forms.Button ();
      this.ConvexHullButon = new System.Windows.Forms.Button ();
      this.BandPassButton = new System.Windows.Forms.Button ();
      this.LowerBound = new System.Windows.Forms.TextBox ();
      this.UpperBound = new System.Windows.Forms.TextBox ();
      this.BinarizeTHButton = new System.Windows.Forms.Button ();
      this.ThLowerBound = new System.Windows.Forms.TextBox ();
      this.ThUpperBound = new System.Windows.Forms.TextBox ();
      this.menuStrip1 = new System.Windows.Forms.MenuStrip ();
      this.File = new System.Windows.Forms.ToolStripMenuItem ();
      this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem ();
      this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem ();
      this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem ();
      this.TrainLib2Name = new System.Windows.Forms.TextBox ();
      this.lib2Pred1Prob = new System.Windows.Forms.TextBox ();
      this.lib2Pred1ClassName = new System.Windows.Forms.TextBox ();
      this.Scale = new System.Windows.Forms.ComboBox ();
      this.ScaleLabel = new System.Windows.Forms.Label ();
      this.FlowRateFactoeLabel = new System.Windows.Forms.Label ();
      this.FlowRateFactor = new System.Windows.Forms.NumericUpDown ();
      this.BackGroundTH = new System.Windows.Forms.NumericUpDown ();
      this.BackGroundTHLabel = new System.Windows.Forms.Label ();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit ();
      this.ImagePanel.SuspendLayout ();
      this.StructuredOperations.SuspendLayout ();
      this.menuStrip1.SuspendLayout ();
      ((System.ComponentModel.ISupportInitialize)(this.FlowRateFactor)).BeginInit ();
      ((System.ComponentModel.ISupportInitialize)(this.BackGroundTH)).BeginInit ();
      this.SuspendLayout ();
      // 
      // pictureBox1
      // 
      this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.pictureBox1.Location = new System.Drawing.Point (3, 3);
      this.pictureBox1.Name = "pictureBox1";
      this.pictureBox1.Size = new System.Drawing.Size (666, 572);
      this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
      this.pictureBox1.TabIndex = 0;
      this.pictureBox1.TabStop = false;
      // 
      // ImagePanel
      // 
      this.ImagePanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.ImagePanel.Controls.Add (this.pictureBox1);
      this.ImagePanel.Location = new System.Drawing.Point (204, 100);
      this.ImagePanel.Name = "ImagePanel";
      this.ImagePanel.Size = new System.Drawing.Size (674, 582);
      this.ImagePanel.TabIndex = 2;
      // 
      // pred1ClassName
      // 
      this.pred1ClassName.Location = new System.Drawing.Point (400, 34);
      this.pred1ClassName.Name = "pred1ClassName";
      this.pred1ClassName.ReadOnly = true;
      this.pred1ClassName.Size = new System.Drawing.Size (252, 20);
      this.pred1ClassName.TabIndex = 7;
      // 
      // pred1Prob
      // 
      this.pred1Prob.Location = new System.Drawing.Point (354, 34);
      this.pred1Prob.Name = "pred1Prob";
      this.pred1Prob.ReadOnly = true;
      this.pred1Prob.Size = new System.Drawing.Size (43, 20);
      this.pred1Prob.TabIndex = 9;
      // 
      // TrainLib1Name
      // 
      this.TrainLib1Name.Location = new System.Drawing.Point (204, 34);
      this.TrainLib1Name.Name = "TrainLib1Name";
      this.TrainLib1Name.ReadOnly = true;
      this.TrainLib1Name.Size = new System.Drawing.Size (135, 20);
      this.TrainLib1Name.TabIndex = 15;
      this.TrainLib1Name.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // Colorize
      // 
      this.Colorize.AutoSize = true;
      this.Colorize.Location = new System.Drawing.Point (122, 34);
      this.Colorize.Name = "Colorize";
      this.Colorize.Size = new System.Drawing.Size (65, 18);
      this.Colorize.TabIndex = 17;
      this.Colorize.Text = "Colorize";
      this.Colorize.UseCompatibleTextRendering = true;
      this.Colorize.UseVisualStyleBackColor = true;
      this.Colorize.CheckedChanged += new System.EventHandler (this.Colorize_CheckedChanged);
      // 
      // ClossingButton
      // 
      this.ClossingButton.Location = new System.Drawing.Point (3, 3);
      this.ClossingButton.Name = "ClossingButton";
      this.ClossingButton.Size = new System.Drawing.Size (81, 22);
      this.ClossingButton.TabIndex = 18;
      this.ClossingButton.Text = "Closing";
      this.ClossingButton.UseCompatibleTextRendering = true;
      this.ClossingButton.UseVisualStyleBackColor = true;
      this.ClossingButton.Click += new System.EventHandler (this.ClossingButton_Click);
      // 
      // OpeningButton
      // 
      this.OpeningButton.Location = new System.Drawing.Point (95, 3);
      this.OpeningButton.Name = "OpeningButton";
      this.OpeningButton.Size = new System.Drawing.Size (76, 22);
      this.OpeningButton.TabIndex = 19;
      this.OpeningButton.Text = "Opening";
      this.OpeningButton.UseCompatibleTextRendering = true;
      this.OpeningButton.UseVisualStyleBackColor = true;
      this.OpeningButton.Click += new System.EventHandler (this.OpeningButton_Click);
      // 
      // ErosionButton
      // 
      this.ErosionButton.Location = new System.Drawing.Point (3, 31);
      this.ErosionButton.Name = "ErosionButton";
      this.ErosionButton.Size = new System.Drawing.Size (81, 22);
      this.ErosionButton.TabIndex = 20;
      this.ErosionButton.Text = "Erosion";
      this.ErosionButton.UseCompatibleTextRendering = true;
      this.ErosionButton.UseVisualStyleBackColor = true;
      this.ErosionButton.Click += new System.EventHandler (this.ErosionButton_Click);
      // 
      // DialationButton
      // 
      this.DialationButton.Location = new System.Drawing.Point (95, 31);
      this.DialationButton.Name = "DialationButton";
      this.DialationButton.Size = new System.Drawing.Size (76, 22);
      this.DialationButton.TabIndex = 21;
      this.DialationButton.Text = "Dialation";
      this.DialationButton.UseCompatibleTextRendering = true;
      this.DialationButton.UseVisualStyleBackColor = true;
      this.DialationButton.Click += new System.EventHandler (this.DialationButton_Click);
      // 
      // EdgeButton
      // 
      this.EdgeButton.Location = new System.Drawing.Point (16, 292);
      this.EdgeButton.Name = "EdgeButton";
      this.EdgeButton.Size = new System.Drawing.Size (76, 22);
      this.EdgeButton.TabIndex = 22;
      this.EdgeButton.Text = "Edge";
      this.EdgeButton.UseCompatibleTextRendering = true;
      this.EdgeButton.UseVisualStyleBackColor = true;
      this.EdgeButton.Click += new System.EventHandler (this.EdgeButton_Click);
      // 
      // FillHolesButton
      // 
      this.FillHolesButton.Location = new System.Drawing.Point (108, 292);
      this.FillHolesButton.Name = "FillHolesButton";
      this.FillHolesButton.Size = new System.Drawing.Size (76, 22);
      this.FillHolesButton.TabIndex = 23;
      this.FillHolesButton.Text = "Fill Holes";
      this.FillHolesButton.UseCompatibleTextRendering = true;
      this.FillHolesButton.UseVisualStyleBackColor = true;
      this.FillHolesButton.Click += new System.EventHandler (this.FillHolesButton_Click);
      // 
      // SmoothingButton
      // 
      this.SmoothingButton.Location = new System.Drawing.Point (3, 59);
      this.SmoothingButton.Name = "SmoothingButton";
      this.SmoothingButton.Size = new System.Drawing.Size (81, 22);
      this.SmoothingButton.TabIndex = 24;
      this.SmoothingButton.Text = "Smoothing";
      this.SmoothingButton.UseCompatibleTextRendering = true;
      this.SmoothingButton.UseVisualStyleBackColor = true;
      this.SmoothingButton.Click += new System.EventHandler (this.SmoothingButton_Click);
      // 
      // MediumButton
      // 
      this.MediumButton.Location = new System.Drawing.Point (95, 59);
      this.MediumButton.Name = "MediumButton";
      this.MediumButton.Size = new System.Drawing.Size (76, 22);
      this.MediumButton.TabIndex = 25;
      this.MediumButton.Text = "Medium";
      this.MediumButton.UseCompatibleTextRendering = true;
      this.MediumButton.UseVisualStyleBackColor = true;
      this.MediumButton.Click += new System.EventHandler (this.MediumButton_Click);
      // 
      // StructuredOperations
      // 
      this.StructuredOperations.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.StructuredOperations.Controls.Add (this.MaskExcludeButton);
      this.StructuredOperations.Controls.Add (this.MaskTypesComboBox);
      this.StructuredOperations.Controls.Add (this.OpeningButton);
      this.StructuredOperations.Controls.Add (this.MediumButton);
      this.StructuredOperations.Controls.Add (this.ClossingButton);
      this.StructuredOperations.Controls.Add (this.SmoothingButton);
      this.StructuredOperations.Controls.Add (this.ErosionButton);
      this.StructuredOperations.Controls.Add (this.DialationButton);
      this.StructuredOperations.Location = new System.Drawing.Point (16, 100);
      this.StructuredOperations.Name = "StructuredOperations";
      this.StructuredOperations.Size = new System.Drawing.Size (174, 170);
      this.StructuredOperations.TabIndex = 26;
      // 
      // MaskExcludeButton
      // 
      this.MaskExcludeButton.Location = new System.Drawing.Point (3, 87);
      this.MaskExcludeButton.Name = "MaskExcludeButton";
      this.MaskExcludeButton.Size = new System.Drawing.Size (81, 22);
      this.MaskExcludeButton.TabIndex = 27;
      this.MaskExcludeButton.Text = "MaskExclude";
      this.MaskExcludeButton.UseCompatibleTextRendering = true;
      this.MaskExcludeButton.UseVisualStyleBackColor = true;
      this.MaskExcludeButton.Click += new System.EventHandler (this.MaskExcludeButton_Click);
      // 
      // MaskTypesComboBox
      // 
      this.MaskTypesComboBox.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.MaskTypesComboBox.FormattingEnabled = true;
      this.MaskTypesComboBox.Location = new System.Drawing.Point (27, 131);
      this.MaskTypesComboBox.Name = "MaskTypesComboBox";
      this.MaskTypesComboBox.Size = new System.Drawing.Size (121, 24);
      this.MaskTypesComboBox.TabIndex = 26;
      // 
      // ThinningButton
      // 
      this.ThinningButton.Location = new System.Drawing.Point (16, 320);
      this.ThinningButton.Name = "ThinningButton";
      this.ThinningButton.Size = new System.Drawing.Size (76, 22);
      this.ThinningButton.TabIndex = 27;
      this.ThinningButton.Text = "Thinning";
      this.ThinningButton.UseCompatibleTextRendering = true;
      this.ThinningButton.UseVisualStyleBackColor = true;
      this.ThinningButton.Click += new System.EventHandler (this.ThinningButton_Click);
      // 
      // UndoButton
      // 
      this.UndoButton.Location = new System.Drawing.Point (61, 438);
      this.UndoButton.Name = "UndoButton";
      this.UndoButton.Size = new System.Drawing.Size (76, 22);
      this.UndoButton.TabIndex = 28;
      this.UndoButton.Text = "Undo";
      this.UndoButton.UseCompatibleTextRendering = true;
      this.UndoButton.UseVisualStyleBackColor = true;
      this.UndoButton.Click += new System.EventHandler (this.UndoButton_Click);
      // 
      // OperationsList
      // 
      this.OperationsList.Font = new System.Drawing.Font ("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.OperationsList.FormattingEnabled = true;
      this.OperationsList.ItemHeight = 16;
      this.OperationsList.Location = new System.Drawing.Point (13, 498);
      this.OperationsList.MultiColumn = true;
      this.OperationsList.Name = "OperationsList";
      this.OperationsList.Size = new System.Drawing.Size (174, 180);
      this.OperationsList.TabIndex = 29;
      // 
      // ConnectedComponentButton
      // 
      this.ConnectedComponentButton.Location = new System.Drawing.Point (108, 320);
      this.ConnectedComponentButton.Name = "ConnectedComponentButton";
      this.ConnectedComponentButton.Size = new System.Drawing.Size (76, 22);
      this.ConnectedComponentButton.TabIndex = 30;
      this.ConnectedComponentButton.Text = "Connected";
      this.ConnectedComponentButton.UseCompatibleTextRendering = true;
      this.ConnectedComponentButton.UseVisualStyleBackColor = true;
      this.ConnectedComponentButton.Click += new System.EventHandler (this.ConnectedComponentButton_Click);
      // 
      // BinarizeButton
      // 
      this.BinarizeButton.Location = new System.Drawing.Point (16, 348);
      this.BinarizeButton.Name = "BinarizeButton";
      this.BinarizeButton.Size = new System.Drawing.Size (76, 22);
      this.BinarizeButton.TabIndex = 31;
      this.BinarizeButton.Text = "Binarize";
      this.BinarizeButton.UseCompatibleTextRendering = true;
      this.BinarizeButton.UseVisualStyleBackColor = true;
      this.BinarizeButton.Click += new System.EventHandler (this.BinarizeButton_Click);
      // 
      // ConvexHullButon
      // 
      this.ConvexHullButon.Location = new System.Drawing.Point (108, 348);
      this.ConvexHullButon.Name = "ConvexHullButon";
      this.ConvexHullButon.Size = new System.Drawing.Size (76, 22);
      this.ConvexHullButon.TabIndex = 32;
      this.ConvexHullButon.Text = "ConvexHull";
      this.ConvexHullButon.UseCompatibleTextRendering = true;
      this.ConvexHullButon.UseVisualStyleBackColor = true;
      this.ConvexHullButon.Click += new System.EventHandler (this.ConvexHullButon_Click);
      // 
      // BandPassButton
      // 
      this.BandPassButton.Location = new System.Drawing.Point (16, 379);
      this.BandPassButton.Name = "BandPassButton";
      this.BandPassButton.Size = new System.Drawing.Size (76, 22);
      this.BandPassButton.TabIndex = 33;
      this.BandPassButton.Text = "Band Pass";
      this.BandPassButton.UseCompatibleTextRendering = true;
      this.BandPassButton.UseVisualStyleBackColor = true;
      this.BandPassButton.Click += new System.EventHandler (this.BandPassButton_Click);
      // 
      // LowerBound
      // 
      this.LowerBound.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.LowerBound.Location = new System.Drawing.Point (108, 379);
      this.LowerBound.Name = "LowerBound";
      this.LowerBound.Size = new System.Drawing.Size (29, 22);
      this.LowerBound.TabIndex = 34;
      this.LowerBound.Text = "0";
      // 
      // UpperBound
      // 
      this.UpperBound.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.UpperBound.Location = new System.Drawing.Point (148, 379);
      this.UpperBound.Name = "UpperBound";
      this.UpperBound.Size = new System.Drawing.Size (31, 22);
      this.UpperBound.TabIndex = 35;
      this.UpperBound.Text = "100";
      // 
      // BinarizeTHButton
      // 
      this.BinarizeTHButton.Location = new System.Drawing.Point (16, 410);
      this.BinarizeTHButton.Name = "BinarizeTHButton";
      this.BinarizeTHButton.Size = new System.Drawing.Size (76, 22);
      this.BinarizeTHButton.TabIndex = 32;
      this.BinarizeTHButton.Text = "Binarize TH";
      this.BinarizeTHButton.UseCompatibleTextRendering = true;
      this.BinarizeTHButton.UseVisualStyleBackColor = true;
      this.BinarizeTHButton.Click += new System.EventHandler (this.BinarizeTHButton_Click);
      // 
      // ThLowerBound
      // 
      this.ThLowerBound.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ThLowerBound.Location = new System.Drawing.Point (108, 410);
      this.ThLowerBound.Name = "ThLowerBound";
      this.ThLowerBound.Size = new System.Drawing.Size (29, 22);
      this.ThLowerBound.TabIndex = 36;
      this.ThLowerBound.Text = "0";
      // 
      // ThUpperBound
      // 
      this.ThUpperBound.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ThUpperBound.Location = new System.Drawing.Point (148, 411);
      this.ThUpperBound.Name = "ThUpperBound";
      this.ThUpperBound.Size = new System.Drawing.Size (30, 22);
      this.ThUpperBound.TabIndex = 37;
      this.ThUpperBound.Text = "0";
      // 
      // menuStrip1
      // 
      this.menuStrip1.Items.AddRange (new System.Windows.Forms.ToolStripItem[] {
            this.File});
      this.menuStrip1.Location = new System.Drawing.Point (0, 0);
      this.menuStrip1.Name = "menuStrip1";
      this.menuStrip1.Size = new System.Drawing.Size (905, 24);
      this.menuStrip1.TabIndex = 38;
      this.menuStrip1.Text = "menuStrip1";
      // 
      // File
      // 
      this.File.DropDownItems.AddRange (new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1,
            this.saveToolStripMenuItem,
            this.exitToolStripMenuItem});
      this.File.Name = "File";
      this.File.Size = new System.Drawing.Size (37, 20);
      this.File.Text = "File";
      // 
      // toolStripMenuItem1
      // 
      this.toolStripMenuItem1.Name = "toolStripMenuItem1";
      this.toolStripMenuItem1.Size = new System.Drawing.Size (103, 22);
      this.toolStripMenuItem1.Text = "Open";
      // 
      // saveToolStripMenuItem
      // 
      this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
      this.saveToolStripMenuItem.Size = new System.Drawing.Size (103, 22);
      this.saveToolStripMenuItem.Text = "Save";
      this.saveToolStripMenuItem.Click += new System.EventHandler (this.saveToolStripMenuItem_Click);
      // 
      // exitToolStripMenuItem
      // 
      this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
      this.exitToolStripMenuItem.Size = new System.Drawing.Size (103, 22);
      this.exitToolStripMenuItem.Text = "Exit";
      this.exitToolStripMenuItem.Click += new System.EventHandler (this.exitToolStripMenuItem_Click);
      // 
      // TrainLib2Name
      // 
      this.TrainLib2Name.Location = new System.Drawing.Point (204, 60);
      this.TrainLib2Name.Name = "TrainLib2Name";
      this.TrainLib2Name.ReadOnly = true;
      this.TrainLib2Name.Size = new System.Drawing.Size (135, 20);
      this.TrainLib2Name.TabIndex = 41;
      this.TrainLib2Name.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // lib2Pred1Prob
      // 
      this.lib2Pred1Prob.Location = new System.Drawing.Point (354, 60);
      this.lib2Pred1Prob.Name = "lib2Pred1Prob";
      this.lib2Pred1Prob.ReadOnly = true;
      this.lib2Pred1Prob.Size = new System.Drawing.Size (43, 20);
      this.lib2Pred1Prob.TabIndex = 40;
      // 
      // lib2Pred1ClassName
      // 
      this.lib2Pred1ClassName.Location = new System.Drawing.Point (400, 60);
      this.lib2Pred1ClassName.Name = "lib2Pred1ClassName";
      this.lib2Pred1ClassName.ReadOnly = true;
      this.lib2Pred1ClassName.Size = new System.Drawing.Size (252, 20);
      this.lib2Pred1ClassName.TabIndex = 39;
      // 
      // Scale
      // 
      this.Scale.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Scale.FormattingEnabled = true;
      this.Scale.Items.AddRange (new object[] {
            "10%",
            "25%",
            "50%",
            "75%",
            "100%",
            "150%",
            "200%",
            "300%",
            "400%"});
      this.Scale.Location = new System.Drawing.Point (55, 30);
      this.Scale.Name = "Scale";
      this.Scale.Size = new System.Drawing.Size (53, 24);
      this.Scale.TabIndex = 42;
      this.Scale.SelectedIndexChanged += new System.EventHandler (this.Scale_SelectedIndexChanged);
      // 
      // ScaleLabel
      // 
      this.ScaleLabel.AutoSize = true;
      this.ScaleLabel.Location = new System.Drawing.Point (16, 37);
      this.ScaleLabel.Name = "ScaleLabel";
      this.ScaleLabel.Size = new System.Drawing.Size (33, 17);
      this.ScaleLabel.TabIndex = 43;
      this.ScaleLabel.Text = "Scale";
      this.ScaleLabel.UseCompatibleTextRendering = true;
      // 
      // FlowRateFactoeLabel
      // 
      this.FlowRateFactoeLabel.AutoSize = true;
      this.FlowRateFactoeLabel.Location = new System.Drawing.Point (16, 63);
      this.FlowRateFactoeLabel.Name = "FlowRateFactoeLabel";
      this.FlowRateFactoeLabel.Size = new System.Drawing.Size (90, 17);
      this.FlowRateFactoeLabel.TabIndex = 44;
      this.FlowRateFactoeLabel.Text = "Flow Rate Factor";
      this.FlowRateFactoeLabel.UseCompatibleTextRendering = true;
      // 
      // FlowRateFactor
      // 
      this.FlowRateFactor.DecimalPlaces = 1;
      this.FlowRateFactor.Font = new System.Drawing.Font ("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.FlowRateFactor.Increment = new decimal (new int[] {
            1,
            0,
            0,
            65536});
      this.FlowRateFactor.Location = new System.Drawing.Point (122, 60);
      this.FlowRateFactor.Maximum = new decimal (new int[] {
            6,
            0,
            0,
            0});
      this.FlowRateFactor.Minimum = new decimal (new int[] {
            1,
            0,
            0,
            65536});
      this.FlowRateFactor.Name = "FlowRateFactor";
      this.FlowRateFactor.Size = new System.Drawing.Size (62, 22);
      this.FlowRateFactor.TabIndex = 45;
      this.FlowRateFactor.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.FlowRateFactor.Value = new decimal (new int[] {
            10,
            0,
            0,
            65536});
      this.FlowRateFactor.ValueChanged += new System.EventHandler (this.FlowRateFactor_ValueChanged);
      // 
      // BackGroundTH
      // 
      this.BackGroundTH.Location = new System.Drawing.Point (101, 471);
      this.BackGroundTH.Maximum = new decimal (new int[] {
            255,
            0,
            0,
            0});
      this.BackGroundTH.Name = "BackGroundTH";
      this.BackGroundTH.Size = new System.Drawing.Size (48, 20);
      this.BackGroundTH.TabIndex = 46;
      this.BackGroundTH.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      this.BackGroundTH.Value = new decimal (new int[] {
            31,
            0,
            0,
            0});
      // 
      // BackGroundTHLabel
      // 
      this.BackGroundTHLabel.AutoSize = true;
      this.BackGroundTHLabel.Location = new System.Drawing.Point (12, 474);
      this.BackGroundTHLabel.Name = "BackGroundTHLabel";
      this.BackGroundTHLabel.Size = new System.Drawing.Size (83, 17);
      this.BackGroundTHLabel.TabIndex = 47;
      this.BackGroundTHLabel.Text = "Background TH";
      this.BackGroundTHLabel.UseCompatibleTextRendering = true;
      // 
      // ImageEditor
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (905, 711);
      this.Controls.Add (this.BackGroundTHLabel);
      this.Controls.Add (this.BackGroundTH);
      this.Controls.Add (this.FlowRateFactor);
      this.Controls.Add (this.FlowRateFactoeLabel);
      this.Controls.Add (this.ScaleLabel);
      this.Controls.Add (this.Scale);
      this.Controls.Add (this.TrainLib2Name);
      this.Controls.Add (this.lib2Pred1Prob);
      this.Controls.Add (this.lib2Pred1ClassName);
      this.Controls.Add (this.ThUpperBound);
      this.Controls.Add (this.ThLowerBound);
      this.Controls.Add (this.BinarizeTHButton);
      this.Controls.Add (this.UpperBound);
      this.Controls.Add (this.LowerBound);
      this.Controls.Add (this.BandPassButton);
      this.Controls.Add (this.ConvexHullButon);
      this.Controls.Add (this.BinarizeButton);
      this.Controls.Add (this.ConnectedComponentButton);
      this.Controls.Add (this.OperationsList);
      this.Controls.Add (this.UndoButton);
      this.Controls.Add (this.ThinningButton);
      this.Controls.Add (this.StructuredOperations);
      this.Controls.Add (this.FillHolesButton);
      this.Controls.Add (this.EdgeButton);
      this.Controls.Add (this.Colorize);
      this.Controls.Add (this.TrainLib1Name);
      this.Controls.Add (this.pred1Prob);
      this.Controls.Add (this.pred1ClassName);
      this.Controls.Add (this.ImagePanel);
      this.Controls.Add (this.menuStrip1);
      this.MainMenuStrip = this.menuStrip1;
      this.Name = "ImageEditor";
      this.Text = "ImageEditor";
      this.Load += new System.EventHandler (this.ImageEditor_Load);
      this.SizeChanged += new System.EventHandler (this.ImageEditor_SizeChanged);
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit ();
      this.ImagePanel.ResumeLayout (false);
      this.ImagePanel.PerformLayout ();
      this.StructuredOperations.ResumeLayout (false);
      this.menuStrip1.ResumeLayout (false);
      this.menuStrip1.PerformLayout ();
      ((System.ComponentModel.ISupportInitialize)(this.FlowRateFactor)).EndInit ();
      ((System.ComponentModel.ISupportInitialize)(this.BackGroundTH)).EndInit ();
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

		private System.Windows.Forms.PictureBox pictureBox1;
		private System.Windows.Forms.Panel ImagePanel;
		private System.Windows.Forms.TextBox pred1ClassName;
		private System.Windows.Forms.TextBox pred1Prob;
		private System.Windows.Forms.TextBox TrainLib1Name;
    private System.Windows.Forms.CheckBox Colorize;
    private System.Windows.Forms.Button ClossingButton;
    private System.Windows.Forms.Button OpeningButton;
    private System.Windows.Forms.Button ErosionButton;
    private System.Windows.Forms.Button DialationButton;
    private System.Windows.Forms.Button EdgeButton;
    private System.Windows.Forms.Button FillHolesButton;
    private System.Windows.Forms.Button SmoothingButton;
    private System.Windows.Forms.Button MediumButton;
    private System.Windows.Forms.Panel StructuredOperations;
    private System.Windows.Forms.ComboBox MaskTypesComboBox;
    private System.Windows.Forms.Button ThinningButton;
    private System.Windows.Forms.Button UndoButton;
    private System.Windows.Forms.ListBox OperationsList;
    private System.Windows.Forms.Button ConnectedComponentButton;
    private System.Windows.Forms.Button BinarizeButton;
    private System.Windows.Forms.Button ConvexHullButon;
      private System.Windows.Forms.Button BandPassButton;
    private System.Windows.Forms.TextBox LowerBound;
    private System.Windows.Forms.TextBox UpperBound;
    private System.Windows.Forms.Button BinarizeTHButton;
    private System.Windows.Forms.TextBox ThLowerBound;
    private System.Windows.Forms.TextBox ThUpperBound;
		private System.Windows.Forms.MenuStrip menuStrip1;
		private System.Windows.Forms.ToolStripMenuItem File;
		private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
		private System.Windows.Forms.TextBox TrainLib2Name;
		private System.Windows.Forms.TextBox lib2Pred1Prob;
		private System.Windows.Forms.TextBox lib2Pred1ClassName;
		private System.Windows.Forms.ComboBox Scale;
		private System.Windows.Forms.Label ScaleLabel;
		private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
    private System.Windows.Forms.Label FlowRateFactoeLabel;
    private System.Windows.Forms.NumericUpDown FlowRateFactor;
    private System.Windows.Forms.Button MaskExcludeButton;
    private System.Windows.Forms.NumericUpDown BackGroundTH;
    private System.Windows.Forms.Label BackGroundTHLabel;
  }
}